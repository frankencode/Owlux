#include <owlux/DiscoveryItem>
#include <cc/Text>
#include <cc/Picture>
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
    }

    Picture onIcon_;
    Property<YeelightStatus> status;
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

DiscoveryItem::State &DiscoveryItem::me()
{
    return Object::me.as<State>();
}

const DiscoveryItem::State &DiscoveryItem::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::owlux
