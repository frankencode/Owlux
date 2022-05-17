#include <beelight/DiscoveryView>
#include <yee/YeelightDiscovery>
#include <cc/Application>
#include <cc/Thread>
#include <cc/AppBar>
#include <cc/ListMenu>
#include <cc/DEBUG>

namespace cc::yee {

struct DiscoveryView::State final: public View::State
{
    State()
    {
        discoveryThread_ = Thread{[this]{
            for (YeelightStatus status: YeelightDiscovery{500, 1}) {
                Application{}.postEvent([this, status]{
                    statusUpdate(status);
                });
            }
            for (YeelightStatus status: listener_) {
                Application{}.postEvent([this, status]{
                    statusUpdate(status);
                });
            }
        }};

        discoveryThread_.start();

        add(
            AppBar{}
            .associate(&appBar_)
            .title("Beelight")
        );

        addBelow(
            ListMenu{}
            .associate(&listMenu_)
            .pos(0, appBar_.height())
            .size([this]{ return size() - Size{0, appBar_.height()}; })
        );
    }

    ~State()
    {
        listener_.shutdown();
        discoveryThread_.wait();
    }

    void statusUpdate(const YeelightStatus &status)
    {
        Locator target;
        if (statusByAddress_.find(status.address(), &target)) {
            statusByAddress_.valueAt(target) = status;
        }
        else {
            statusByAddress_.insert(status.address(), status, &target);
            listMenu_.carrier().insertAt(target.index(), ListItem{}.title(name(status)));
        }
    }

    static String name(const YeelightStatus &status)
    {
        return status.name() != "" ? status.name() : status.address().networkAddress();
    }

    YeelightDiscovery listener_;
    Thread discoveryThread_;
    Map<SocketAddress, YeelightStatus> statusByAddress_;

    AppBar appBar_;
    ListMenu listMenu_;
};

DiscoveryView::DiscoveryView():
    View{onDemand<State>}
{}

DiscoveryView &DiscoveryView::associate(Out<DiscoveryView> self)
{
    return View::associate<DiscoveryView>(self);
}

} // namespace cc::yee
