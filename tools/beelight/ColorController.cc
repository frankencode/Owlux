#include <beelight/ColorController>
#include <yee/YeelightControl>
#include <yee/YeelightPower>
#include <yee/YeelightSetBrightness>
#include <cc/ColumnLayout>
#include <cc/AppBar>
#include <cc/Text>
#include <cc/Switch>
#include <cc/Slider>
#include <cc/Icon>
#include <cc/DEBUG>

namespace cc::yee {

struct ColorController::State: public View::State
{
    State(const YeelightStatus &status):
        control_{status.address()}
    {
        add(
            AppBar{}
            .associate(&appBar_)
            .title(
                status.name() != "" ?
                Format{"Light \"%%\""}.arg(status.name()) :
                Format{"Light %%"}.arg(status.id())
            )
        );

        addBelow(
            View{}
            .pos(0, appBar_.height())
            .size([this]{ return size() - Size{0, appBar_.height()}; })
            .layout(ColumnLayout{})
            .add(
                Switch{"Power"}
                .associate(&powerSwitch_)
                .value(status.power())
                .onValueChanged([this]{
                    control_.requestChannel().pushBack(YeelightPower{powerSwitch_.value()});
                })
            )
            .add(
                Slider{}
                .associate(&brightSlider_)
                .leading(Icon{Ideographic::Brightness5})
                .trailing(Icon{Ideographic::Brightness7})
                .min(0)
                .max(100)
                .value(status.brightness())
                .onValueChanged([this]{
                    int newValue = static_cast<int>(brightSlider_.value());
                    CC_INSPECT(newValue);
                    control_.requestChannel().pushExclusive(YeelightSetBrightness{newValue});
                })
            )
        );
    }

    AppBar appBar_;
    Switch powerSwitch_;
    Slider brightSlider_;
    YeelightControl control_;
};

ColorController::ColorController(const YeelightStatus &status):
    View{new State{status}}
{}

ColorController &ColorController::associate(Out<ColorController> self)
{
    return View::associate<ColorController>(self);
}

ColorController &ColorController::onDismissed(Fun<void()> &&f)
{
    me().appBar_.onDismissed(move(f));
    return *this;
}

ColorController::State &ColorController::me()
{
    return get<State>();
}

const ColorController::State &ColorController::me() const
{
    return get<State>();
}

} // namespace cc::yee
