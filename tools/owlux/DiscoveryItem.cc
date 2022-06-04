#include <owlux/DiscoveryItem>
#include <cc/Text>
#include <cc/Picture>
#include <cc/Timer>
#include <cc/DEBUG>

namespace cc::owlux {

struct DiscoveryItem::State final: public Control::State
{
    State(const YeelightStatus &initialStatus):
        status{initialStatus}
    {
        paper([this]{
            return pressed() ? theme().itemHighlightColor() : Color{};
        });

        add(
            Picture{Ideographic::LightbulbOnOutline}
            .associate(&onIcon_)
            .centerLeft([this]{ return Point{sp(16), height() / 2}; })
            .visible([this]{ return status().power(); })
        );

        add(
            Picture{Ideographic::LightbulbOutline}
            .centerLeft([this]{ return Point{sp(16), height() / 2}; })
            .visible([this]{ return !status().power(); })
        );

        add(
            Text{}
            .text([this]{ return status().displayName(); })
            .centerLeft([this]{ return Point{onIcon_.right() + sp(16), height() / 2}; })
        );

        if (initialStatus.refreshInterval() > 0) {
            expiryTimer_ = Timer{static_cast<double>(2 * initialStatus.refreshInterval() + 60)};
                // "2 x refresh interval" because first generation Yeelight model "color" seems to have a bug
                // misreporting its refresh interval as 1 hour, although it refreshes every 2 hours
            expiryTimer_.start();
        }
    }

    void update(const YeelightStatus &status)
    {
        this->status().update(status);
        if (expiryTimer_) expiryTimer_.start();
    }

    Picture onIcon_;
    Property<YeelightStatus> status;
    Timer expiryTimer_;
};

DiscoveryItem::DiscoveryItem(const YeelightStatus &status):
    Control{new State{status}}
{}

DiscoveryItem &DiscoveryItem::associate(Out<DiscoveryItem> self)
{
    return View::associate<DiscoveryItem>(self);
}

YeelightStatus DiscoveryItem::status() const
{
    return me().status();
}

DiscoveryItem &DiscoveryItem::status(const YeelightStatus &status)
{
    me().status(status);
    return *this;
}

DiscoveryItem &DiscoveryItem::onExpired(Fun<void()> &&f)
{
    if (me().expiryTimer_) me().expiryTimer_.onTimeout(move(f));
    return *this;
}

void DiscoveryItem::update(const YeelightStatus &status)
{
    me().update(status);
}

DiscoveryItem::State &DiscoveryItem::me()
{
    return Object::me.as<State>();
}

const DiscoveryItem::State &DiscoveryItem::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::owlux
