#include <owlux/DiscoveryView>
#include <owlux/DiscoveryItem>
#include <owlux/YeelightDiscovery>
#include <cc/Application>
#include <cc/Thread>
#include <cc/AppBar>
#include <cc/ListMenu>
#include <cc/DEBUG>

namespace cc::owlux {

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

        add(
            AppBar{}
            .associate(&appBar_)
            .title("Owlux")
        );

        addBelow(
            ListMenu{}
            .associate(&listMenu_)
            .pos(0, appBar_.height())
            .size([this]{ return size() - Size{0, appBar_.height()}; })
        );

        discoveryThread_.start();
    }

    ~State()
    {
        listener_.shutdown();
        discoveryThread_.wait();
    }

    void statusUpdate(const YeelightStatus &status)
    {
        CC_INSPECT(status);
        Locator target;
        if (itemByAddress_.find(status.address(), &target)) {
            itemByAddress_.at(target).value().status().update(status);
        }
        else {
            DiscoveryItem item;

            listMenu_.carrier().insertAt(
                target.index(),
                DiscoveryItem{status}
                    .associate(&item)
                    .onClicked([this,item]{
                        onSelected_(item.status());
                    })
            );

            itemByAddress_.insert(status.address(), item, &target);
        }
    }

    YeelightDiscovery listener_;
    Thread discoveryThread_;
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
