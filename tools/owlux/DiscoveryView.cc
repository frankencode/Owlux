#include <owlux/DiscoveryView>
#include <owlux/DiscoveryItem>
#include <owlux/YeelightDiscovery>
#include <cc/Application>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/AppBar>
#include <cc/ListMenu>
#include <cc/FloatingButton>
#include <cc/DEBUG>

namespace cc::owlux {

struct DiscoveryView::State final: public View::State
{
    const int MinScanInterval = 3; ///< Number of seconds until another discovery message can be send out

    State()
    {
        scanningThread_ = Thread{[this]{
            while (!scannerShutdown_.tryAcquire()) {
                for (YeelightStatus status: YeelightDiscovery{500, 1}) {
                    Application{}.postEvent([this, status]{
                        statusUpdate(status);
                    });
                }
                scanningRequest_.acquire();
            }
        }};

        discoveryThread_ = Thread{[this]{
            for (YeelightStatus status: listener_) {
                Application{}.postEvent([this, status]{
                    statusUpdate(status);
                });
            }
        }};

        add(
            AppBar{}
            .associate(&appBar_)
            .title("Lights")
            .onNavigate([this]{
                CC_DEBUG;
            })
        );

        addBelow(
            ListMenu{}
            .associate(&listMenu_)
            .pos(0, appBar_.height())
            .size([this]{ return size() - Size{0, appBar_.height()}; })
        );

        add(
            FloatingButton{"SCAN", Ideographic::Reload}
            .onClicked([this]{
                double nowTime = System::now();
                if (nowTime - previousScanTime_ > MinScanInterval) {
                    itemByAddress_.deplete();
                    listMenu_.carrier().deplete();
                    scanningRequest_.release();
                    previousScanTime_ = nowTime;
                }
            })
            .bottomCenter([this]{ return bottomCenter() - Point{0, sp(16)}; })
        );

        scanningThread_.start();
        discoveryThread_.start();
    }

    ~State()
    {
        listener_.shutdown();
        discoveryThread_.wait();

        scannerShutdown_.release();
        scanningRequest_.release();
        scanningThread_.wait();
    }

    void statusUpdate(const YeelightStatus &status)
    {
        // CC_INSPECT(status);
        Locator target;
        if (itemByAddress_.find(status.address(), &target)) {
            itemByAddress_.at(target).value().update(status);
        }
        else {
            DiscoveryItem item{status};

            item
            .onExpired([this,status]{
                Locator target;
                if (itemByAddress_.find(status.address(), &target)) {
                    listMenu_.carrier().remove(itemByAddress_.at(target).value());
                    itemByAddress_.removeAt(target);
                }
            })
            .onClicked([this, status]{
                onSelected_(status);
            });

            itemByAddress_.insert(status.address(), item, &target);
            listMenu_.carrier().insertAt(target.index(), item);
        }
    }

    YeelightDiscovery listener_;
    Thread discoveryThread_;

    double previousScanTime_ { 0 };
    Semaphore<int> scannerShutdown_;
    Semaphore<int> scanningRequest_;
    Thread scanningThread_;

    Map<SocketAddress, DiscoveryItem> itemByAddress_;

    AppBar appBar_;
    ListMenu listMenu_;
    Fun<void(const YeelightStatus &status)> onSelected_;
};

DiscoveryView::DiscoveryView():
    View{onDemand<State>}
{}

DiscoveryView &DiscoveryView::associate(Out<DiscoveryView> self)
{
    return View::associate<DiscoveryView>(self);
}

DiscoveryView &DiscoveryView::onSelected(Fun<void(const YeelightStatus &status)> &&f)
{
    me().onSelected_ = move(f);
    return *this;
}

DiscoveryView::State &DiscoveryView::me()
{
    return get<State>();
}

const DiscoveryView::State &DiscoveryView::me() const
{
    return get<State>();
}

} // namespace cc::owlux
