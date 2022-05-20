#include <beelight/StatusItem>

namespace cc::owlux {

struct StatusItem::State final: public ListItem::State
{
    State(const YeelightStatus &status):
        status_{status}
    {}

    YeelightStatus status_;
};

StatusItem::StatusItem(const YeelightStatus &status):
    ListItem{new State{status}}
{
    title(status.displayName());
}

StatusItem &StatusItem::associate(Out<StatusItem> self)
{
    return View::associate<StatusItem>(self);
}

const YeelightStatus &StatusItem::status() const
{
    return me().status_;
}

StatusItem &StatusItem::status(const YeelightStatus &newValue)
{
    me().status_ = newValue;
    return *this;
}

StatusItem::State &StatusItem::me()
{
    return get<State>();
}

const StatusItem::State &StatusItem::me() const
{
    return get<State>();
}

} // namespace cc::owlux
