#include <beelight/ColorController>
#include <owlux/YeelightControl>
#include <owlux/YeelightPower>
#include <owlux/YeelightSetBrightness>
#include <owlux/YeelightSetColorTemp>
#include <owlux/YeelightSetHueSat>
#include <cc/ColumnLayout>
#include <cc/AppBar>
#include <cc/Text>
#include <cc/Switch>
#include <cc/Slider>
#include <cc/Icon>
#include <cc/NumberCell>
#include <cc/Divider>
#include <cc/Thread>
#include <cc/DEBUG>

namespace cc::owlux {

struct ColorController::State: public View::State
{
    State(const YeelightStatus &status):
        control_{status.address()},
        responseWorker_{[this]{ responseWorker(); }}
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
            .add(Divider{})
            .add(
                Slider{}
                .associate(&brightSlider_)
                .leading(Icon{Ideographic::Brightness6})
                .trailing(NumberCell{})
                .min(1)
                .max(100)
                .value(status.brightness())
                .onValueChanged([this]{
                    control_.requestChannel().pushExclusive(
                        YeelightSetBrightness{
                            static_cast<int>(brightSlider_.value())
                        }
                    );
                })
                .visible(status.supportedMethods().find("set_bright"))
            )
            .add(
                Slider{}
                .associate(&tempSlider_)
                .leading(Icon{Ideographic::Thermometer})
                .trailing(NumberCell{})
                .min(1700)
                .max(6500)
                .value(status.colorTemp())
                .onValueChanged([this]{
                    control_.requestChannel().pushExclusive(
                        YeelightSetColorTemp{
                            static_cast<int>(tempSlider_.value())
                        }
                    );
                })
                .visible(status.supportedMethods().find("set_ct_abx"))
            )
            .add(
                Divider{}
                .visible(brightSlider_.visible() || tempSlider_.visible())
            )
            .add(
                Slider{}
                .associate(&hueSlider_)
                .leading(Text{"Hue"})
                .trailing(NumberCell{})
                .min(0)
                .max(359)
                .value(status.hue())
                .onValueChanged([this]{
                    control_.requestChannel().pushExclusive(
                        YeelightSetHueSat{
                            static_cast<int>(hueSlider_.value()),
                            static_cast<int>(satSlider_.value())
                        }
                    );
                })
                .visible(status.supportedMethods().find("set_hsv"))
            )
            .add(
                Slider{}
                .associate(&satSlider_)
                .leading(Text{"Sat"})
                .trailing(NumberCell{})
                .min(0)
                .max(100)
                .value(status.sat())
                .onValueChanged([this]{
                    control_.requestChannel().pushExclusive(
                        YeelightSetHueSat{
                            static_cast<int>(hueSlider_.value()),
                            static_cast<int>(satSlider_.value())
                        }
                    );
                })
                .visible(hueSlider_.visible())
            )
        );

        responseWorker_.start();
    }

    ~State()
    {
        control_.responseChannel().shutdown();
        responseWorker_.wait();
    }

    void responseWorker()
    {
        for (YeelightResponse response: control_.responseChannel());
        #if 0
        {
            ferr() << response << nl;
        }
        #endif
    }

    AppBar appBar_;
    Switch powerSwitch_;
    Slider brightSlider_;
    Slider tempSlider_;
    Slider hueSlider_;
    Slider satSlider_;
    YeelightControl control_;
    Thread responseWorker_;
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

} // namespace cc::owlux
