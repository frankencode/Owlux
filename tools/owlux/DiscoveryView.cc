/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/DiscoveryView>
#include <owlux/DiscoveryItem>
#include <owlux/YeelightDiscovery>
#include <owlux/YeelightControl>
#include <owlux/YeelightPower>
#include <cc/Application>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/AppBar>
#include <cc/ListMenu>
#include <cc/FloatingButton>
#include <cc/Timer>
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

        offTimeUpdate_.onTimeout([this]{
            double now = System::now();
            for (const auto &pair: itemByAddress_) {
                YeelightStatus status = pair.value().status();
                if (status.hasOffTime()) {
                    if (status.offTime() <= now) {
                        status.unsetOffTime();
                        status.setPower(false);
                    }
                }
            }
        });

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
            FloatingButton{"SCAN", Icon::Reload}
            .onClicked([this]{ scan(); })
            .bottomCenter([this]{ return bottomCenter() - Point{0, sp(16)}; })
        );

        scanningThread_.start();
        discoveryThread_.start();
        offTimeUpdate_.start();
    }

    ~State()
    {
        listener_.shutdown();
        discoveryThread_.wait();

        scannerShutdown_.release();
        scanningRequest_.release();
        scanningThread_.wait();
    }

    void scan()
    {
        double nowTime = System::now();
        if (nowTime - previousScanTime_ > MinScanInterval) {
            itemByAddress_.deplete();
            listMenu_.carrier().deplete();
            scanningRequest_.release();
            previousScanTime_ = nowTime;
        }
    }

    /** \todo improve error handling for power on/off
      */
    void statusUpdate(YeelightStatus status)
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
            .onSettingsRequested([this, status]{
                onSelected_(status);
            })
            .onClicked([this, status]() mutable {
                try {
                    bool on = !status.power();
                    YeelightControl control{status.address()};
                    control.requestChannel().pushBack(YeelightPower{on});
                    if (!control.waitEstablished(1000)) scan();
                    status.setPower(on);
                }
                catch(...)
                {}
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

    Timer offTimeUpdate_ { 3 };

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
