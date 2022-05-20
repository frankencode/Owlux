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

        socket_.shutdown();
        responseConsumer_.wait();
    }

    void runCommandFeed()
    {
        double nextTime = System::now();
        for (YeelightCommand command: requestChannel_) {
            if (command->id_ < 0) command->id_ = nextId_++;
            String json = command.toString();
            double nowTime = System::now();
            if (nextTime - nowTime > 0) {
                if (shutdown_.acquireBefore(nextTime)) break;
                nextTime += timeInterval_;
            }
            else {
                nextTime = nowTime + timeInterval_;
            }
            // CC_INSPECT(json);
            // CC_INSPECT(hex(json));
            socket_.write(json);
        }
    }

    void runResponseConsumer()
    {
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

    YeelightResult execute(const YeelightCommand &command)
    {
        requestChannel_.write(command);

        YeelightResult result;
        for (YeelightResponse response; responseChannel_.read(&response);) {
            result = YeelightResult{response};
            if (result) {
                if (result.commandId() == command.id()) break;
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
    long nextId_ { 1 };
    double timeInterval_ { 1 };
};

YeelightControl::YeelightControl(const SocketAddress &address):
    Object{new State{address}}
{}

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
