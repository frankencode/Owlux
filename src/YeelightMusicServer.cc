#include <owlux/YeelightMusicServerState>

namespace cc::owlux {

YeelightMusicServer::State::State():
    thread_{[this]{ runServer(); }}
{}

YeelightMusicServer::State::~State()
{
    if (!shutdown_.tryAcquire()) {
        shutdown();
        wait();
    }
}

void YeelightMusicServer::State::start()
{
    thread_.start();
}

void YeelightMusicServer::State::shutdown()
{
    shutdown_.release(2);
}

void YeelightMusicServer::State::wait()
{
    thread_.wait();
}

void YeelightMusicServer::State::runServer()
{
    socket_ = ServerSocket{address_};
    ready(socket_.address());
    while (true) {
        bool ready = socket_.wait(IoEvent::ReadyAccept, 1000);
        while (terminated_.count() > 0) {
            YeelightMusicServerConnection c;
            terminated_.popFront(&c);
            connections_.remove(c.client().address());
        }
        if (shutdown_.tryAcquire()) break;
        if (!ready) continue;
        StreamSocket client = socket_.accept();
        YeelightMusicServerConnection connection{client};
        connection.start(Thread{[this, connection]{ runWorker(connection); }});
        connections_.insert(client.address(), connection);
    }
}

void YeelightMusicServer::State::runWorker(const YeelightMusicServerConnection &connection)
{
    StreamSocket client = connection.client();
    SocketAddress address = client.address();
    connected(address);
    try {
        while (true) {
            client.write(serve(address).asBytes());
        }
    }
    catch(...) {
    }
    disconnected(address);
    terminated_.pushBack(connection);
}

YeelightMusicServer::YeelightMusicServer(State *newState):
    Object{newState}
{}

void YeelightMusicServer::start()
{
    me().start();
}

void YeelightMusicServer::shutdown()
{
    me().shutdown();
}

void YeelightMusicServer::wait()
{
    me().wait();
}

} // namespace cc::owlux
