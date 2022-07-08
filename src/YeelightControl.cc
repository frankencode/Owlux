/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightControl>
#include <cc/ClientSocket>
#include <cc/Uri>
#include <cc/System>
#include <cc/Channel>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/Format>
#include <cc/LineSource>
#include <cc/json>
#include <cc/DEBUG>

namespace cc::owlux {

struct YeelightControl::State: public Object::State
{
    State(const SocketAddress &address):
        socket_{address}
    {
        commandFeed_ = Thread{[this]{ runCommandFeed(); }};
        commandFeed_.start();

        responseConsumer_ = Thread{[this]{ runResponseConsumer(); }};
        responseConsumer_.start();
    }

    ~State()
    {
        shutdown_.release();
        requestChannel_.close();
        commandFeed_.wait();
        try {
            socket_.shutdown();
        }
        catch (...)
        {}
        responseConsumer_.wait();
    }

    void runCommandFeed()
    {
        try {
            if (!socket_.waitEstablished(1000)) return;

            double nextTime = System::now();
            do {
                YeelightCommand command;
                if (!requestChannel_.read(&command)) break;
                if (command->id_ < 0) command->id_ = nextId_++;
                nextTime = System::now() + timeInterval_;
                socket_.write(command.toString());
            } while (!shutdown_.acquireBefore(nextTime));
        }
        catch (Exception &ex) {
            CC_INSPECT(ex);
        }
    }

    void runResponseConsumer()
    {
        try {
            if (!socket_.waitEstablished(1000)) return;

            for (String line: LineSource{socket_}) {
                Variant messageValue = jsonParse(line);
                YEELIGHT_EXPECT(messageValue.is<MetaObject>());
                MetaObject message = messageValue.to<MetaObject>();
                if (YeelightResult::recognise(message)) {
                    responseChannel_.write(YeelightResult{message});
                }
                else if (YeelightUpdate::recognise(message)) {
                    responseChannel_.write(YeelightUpdate{message});
                }
            }
        }
        catch (Exception &ex) {
            CC_INSPECT(ex);
            responseChannel_.pushBack(YeelightResponse{});
            responseChannel_.close();
        }
    }

    YeelightResult execute(const YeelightCommand &command)
    {
        if (command.id() < 0) YeelightCommand{command}->id_ = nextId_++;
        requestChannel_.write(command);
        return waitForResult(command.id());
    }

    YeelightResult waitForResult(int commandId)
    {
        YeelightResult result;
        for (YeelightResponse response; responseChannel_.read(&response);) {
            result = YeelightResult{response};
            if (result) {
                if (result.commandId() == commandId) break;
            }
        }
        return result;
    }

    Channel<YeelightCommand> requestChannel_;
    Channel<YeelightResponse> responseChannel_;
    ClientSocket socket_;
    Thread commandFeed_;
    Thread responseConsumer_;
    Semaphore<int> shutdown_;
    std::atomic<int> nextId_ { 1 };
    double timeInterval_ { 1 };
};

YeelightControl::YeelightControl(const SocketAddress &address):
    Object{new State{address}}
{}

bool YeelightControl::waitEstablished(int timeout)
{
    return me().socket_.waitEstablished(timeout);
}

YeelightResult YeelightControl::execute(const YeelightCommand &command)
{
    return me().execute(command);
}

Channel<YeelightCommand> YeelightControl::requestChannel()
{
    return me().requestChannel_;
}

Channel<YeelightResponse> YeelightControl::responseChannel()
{
    return me().responseChannel_;
}

YeelightControl::State &YeelightControl::me()
{
    return Object::me.as<State>();
}

} // namespace cc::
