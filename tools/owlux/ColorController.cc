#include <owlux/ColorController>
#include <owlux/DeviceInspector>
#include <owlux/YeelightControl>
#include <owlux/YeelightPower>
#include <owlux/YeelightSetBrightness>
#include <owlux/YeelightSetColorTemp>
#include <owlux/YeelightSetHueSat>
#include <owlux/YeelightSave>
#include <owlux/YeelightSetName>
#include <cc/Application>
#include <cc/ColumnLayout>
#include <cc/StackView>
#include <cc/AppBar>
#include <cc/Text>
#include <cc/Switch>
#include <cc/Slider>
#include <cc/NumberCell>
#include <cc/ElevatedButton>
#include <cc/Divider>
#include <cc/CustomDialog>
#include <cc/LineEdit>
#include <cc/Thread>
#include <cc/DEBUG>

namespace cc::owlux {

struct ColorController::State final: public View::State
{
    State(const YeelightStatus &status, const StackView &stack):
        stack_{stack},
        control_{status.address()},
        status_{status},
        responseWorker_{[this]{ responseWorker(); }}
    {
        add(
            AppBar{}
            .associate(&appBar_)
            .title([this]{
                return
                    status_.name() != "" ?
                    Format{"Settings for \"%%\""}.arg(status_.name()) :
                    Format{"Settings for %%"}.arg(status_.id());
            })
            .addAction(
                Action{}
                .icon(Icon::Information)
                .onTriggered([this, status]{
                    stack_.push(
                        DeviceInspector{status, stack_}
                    );
                })
            )
            .addAction(
                Action{}
                .icon(Icon::Pencil)
                .onTriggered([this, status]{
                    CustomDialog dialog;
                    LineEdit edit;

                    CustomDialog{}
                    .associate(&dialog)
                    .addContent(
                        LineEdit{}
                        .associate(&edit)
                        .title("Name")
                        .text(status.name())
                        .onAccepted([=,this]() mutable {
                            setName(edit.text());
                            dialog.close();
                        })
                        .focus(true)
                    )
                    .addAction(
                        Action{"Cancel"}
                        ([=]() mutable {
                            CC_DEBUG << "Cancel";
                            dialog.close();
                        })
                    )
                    .addAction(
                        Action{"OK"}
                        ([=,this]() mutable {
                            if (status.name() != edit.text()) {
                                setName(edit.text());
                            }
                            dialog.close();
                        })
                    )
                    .open();
                })
            )
            .onDismissed([this]{
                stack_.pop();
            })
        );

        const bool needSaveButton =
            status.supportedMethods().find("set_default") &&
            status.model() == "color";

        addBelow(
            View{}
            .pos(0, appBar_.height())
            .size([this]{ return size() - Size{0, appBar_.height()}; })
            .layout(ColumnLayout{})
            #if 0
            .add(
                Switch{"Power"}
                .associate(&powerSwitch_)
                .value(status_.power())
                .onUserInput([this]{
                    control_.requestChannel().pushBack(YeelightPower{powerSwitch_.value()});
                    status_.setPower(powerSwitch_.value());
                })
            )
            .add(Divider{})
            #endif
            .add(
                Slider{}
                .associate(&brightSlider_)
                .leading(Icon::Brightness6)
                .trailing(NumberCell{})
                .min(1)
                .max(100)
                .value(status.brightness())
                .onUserInput([this]{
                    pushPoweredRequest(
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
                .leading(Icon::Thermometer)
                .trailing(NumberCell{})
                .min(1700)
                .max(6500)
                .value(status.colorTemp())
                .onValueChanged([this]{
                    pushPoweredRequest(
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
                    pushPoweredRequest(
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
                    pushPoweredRequest(
                        YeelightSetHueSat{
                            static_cast<int>(hueSlider_.value()),
                            static_cast<int>(satSlider_.value())
                        }
                    );
                })
                .visible(hueSlider_.visible())
            )
        );

        if (needSaveButton) {
            add(
                ElevatedButton{"Save as default", Icon::Memory}
                .onClicked([this]{
                    pushPoweredRequest(
                        YeelightSave{}
                    );
                })
                .bottomCenter([this]{
                    return Point{width()/2, height() - sp(16)};
                })
                .visible(needSaveButton)
            );
        }

        responseWorker_.start();
    }

    ~State()
    {
        control_.responseChannel().shutdown();
        responseWorker_.wait();
    }

    void setName(const String &newName)
    {
        control_.requestChannel().pushBack(YeelightSetName{newName});
        status_.setName(newName);
    }

    void pushPoweredRequest(const YeelightCommand &command)
    {
        if (!status_.power()) {
            control_.requestChannel().pushBack(YeelightPower{true});
            control_.requestChannel().pushBack(command);
            powerSwitch_.value(true);
            status_.setPower(true);
        }
        else {
            control_.requestChannel().pushExclusive(command);
        }
    }

    void responseWorker()
    {
        for (YeelightResponse response: control_.responseChannel())
        {
            if (!response) {
                Application{}.postEvent([this]{
                    stack_.pop();
                });
                break;
            }

            // ferr() << response << nl;

            YeelightUpdate update = response;
            if (update) {
                Application{}.postEvent([this, update]{
                    status_.update(update);
                    if (update.hasPowerChanged()) {
                        powerSwitch_.value(update.newPower());
                    }
                    if (update.hasBrightnessChanged() && !brightSlider_.isDragged()) {
                        brightSlider_.value(update.newBrightness());
                    }
                    if (update.hasColorTempChanged() && !tempSlider_.isDragged()) {
                        tempSlider_.value(update.newColorTemp());
                    }
                    if (update.hasHueChanged() && !hueSlider_.isDragged()) {
                        hueSlider_.value(update.newHue());
                    }
                    if (update.hasSatChanged() && !satSlider_.isDragged()) {
                        satSlider_.value(update.newSat());
                    }
                });
            }
        }
    }

    StackView stack_;
    AppBar appBar_;
    Switch powerSwitch_;
    Slider brightSlider_;
    Slider tempSlider_;
    Slider hueSlider_;
    Slider satSlider_;
    YeelightControl control_;
    YeelightStatus status_;
    Thread responseWorker_;
};

ColorController::ColorController(const YeelightStatus &status, const StackView &stack):
    View{new State{status, stack}}
{}

ColorController &ColorController::associate(Out<ColorController> self)
{
    return View::associate<ColorController>(self);
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
