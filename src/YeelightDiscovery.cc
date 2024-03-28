/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightDiscovery>
#include <cc/NetworkInterface>
#include <cc/DatagramSocket>
#include <cc/Semaphore>
#include <cc/System>
#include <cc/Thread>
#include <cc/DEBUG>

namespace cc::owlux {

struct YeelightDiscovery::State: public Object::State
{
    State(long refreshInterval, int maxRetry):
        clientSocket_{guessClientSocketAddress()},
        refreshInterval_{refreshInterval},
        retryCount_{maxRetry}
    {
        if (refreshInterval_ > 0) {
            serverSocket_ = clientSocket_;
            clientThread_ = Thread{[this]{ runClient(); }};
            clientThread_.start();
        }
        else {
            serverSocket_ = DatagramSocket{discoveryAddress_};
            serverSocket_.join(groupAddress_);
            serverThread_ = Thread{[this]{ runServer(); }};
            serverThread_.start();
        }
    }

    ~State()
    {
        wait();
    }

    static SocketAddress guessClientSocketAddress()
    {
        SocketAddress address;
        List<NetworkInterface> interfaceList = NetworkInterface::queryAll(ProtocolFamily::InternetV4);
        for (const NetworkInterface &interface: interfaceList) {
            if (interface.name().startsWith("wl")) {
                List<NetworkLabel> labels = interface.labels();
                if (labels.count() > 0) {
                    address = labels.at(0).localAddress();
                    break;
                }
            }
        }
        if (!address) address = SocketAddress{ProtocolFamily::InternetV4, "*"};
        return address;
    }

    void refresh()
    {
        clientSocket_.send(discoveryMessage_, discoveryAddress_);
    }

    void wait()
    {
        if (clientThread_) {
            clientShutdown_.release();
            clientThread_.wait();
            clientThread_ = Thread{};
        }
    }

    void shutdown()
    {
        if (clientThread_) {
            clientSocket_.shutdown(IoShutdown::Write);
            clientShutdown_.release();
        }
        else {
            serverShutdown_.release();
        }
    }

    void runClient()
    {
        serverThread_ = Thread{[this]{ runServer(); }};
        serverThread_.start();

        try {
            for (; retryCount_ != 0; retryCount_ -= (retryCount_ > 0)) {
                refresh();
                refresh();
                if (clientShutdown_.acquireBefore(System::now() + refreshInterval_ / 1000.)) break;
            }
            serverShutdown_.release();
        }
        catch (...)
        {}

        serverThread_.wait();
    }

    void runServer()
    {
        try {
            String buffer = String::allocate(0x999);
            while (true) {
                if (!serverSocket_.wait(IoEvent::ReadyRead, 1000)) {
                    if (serverShutdown_.tryAcquire()) break;
                    continue;
                }
                SocketAddress address;
                int bytesRead = serverSocket_.receive(&buffer, &address);
                if (bytesRead == 0) break;
                String message = buffer.select(0, bytesRead);
                if (!message.startsWith("M-SEARCH")) {
                    try {
                        result_.emplaceBack(address, message);
                    }
                    catch (Exception &ex) {
                        CC_INSPECT(ex);
                    }
                }
            }
        }
        catch (Exception &ex)
        {
            CC_INSPECT(ex);
        }
        catch (...)
        {}

        result_.close();
    }

    SocketAddress groupAddress_ {
        ProtocolFamily::InternetV4, "239.255.255.250"
    };

    SocketAddress discoveryAddress_{
        ProtocolFamily::InternetV4, "239.255.255.250", 1982
    };

    Channel<YeelightStatus> result_;
    DatagramSocket clientSocket_;
    DatagramSocket serverSocket_;
    IoStream serverMaster_;
    IoStream serverSlave_;
    long refreshInterval_ { 3000 };
    int retryCount_ { -1 };
    Thread serverThread_;
    Thread clientThread_;
    Semaphore<int> clientShutdown_;
    Semaphore<int> serverShutdown_;

    String discoveryMessage_ {
        "M-SEARCH * HTTP/1.1\r\n"
        "HOST: 239.255.255.250:1982\r\n"
        "MAN: \"ssdp:discover\"\r\n"
        "ST: wifi_bulb\r\n"
    };
};

YeelightDiscovery::YeelightDiscovery(long refreshInterval, int maxRetry):
    Object{new State{refreshInterval, maxRetry}}
{}

void YeelightDiscovery::refresh()
{
    me().refresh();
}

void YeelightDiscovery::wait()
{
    me().wait();
}

void YeelightDiscovery::shutdown()
{
    me().shutdown();
}

Channel<YeelightStatus> &YeelightDiscovery::discovery()
{
    return me().result_;
}

YeelightDiscovery::State &YeelightDiscovery::me()
{
    return Object::me.as<State>();
}

} // namespace cc::owlux
