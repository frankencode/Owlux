#include <yee/YeelightUpdate>
#include <yee/YeelightProtocolError>
#include <cc/MetaObject>
#include <cc/json>

namespace cc::yee {

struct YeelightUpdate::State: public YeelightResponse::State
{
    explicit State(const MetaObject &message):
        message_{message}
    {
        YEELIGHT_EXPECT(YeelightUpdate::recognise(message));

        Variant params;
        YEELIGHT_EXPECT(message.lookup("params", &params));
        YEELIGHT_EXPECT(params.is<MetaObject>());
        change_ = params.to<MetaObject>().members();
    }

    String toString() const override
    {
        return jsonStringify(message_);
    }

    MetaObject message_;
    Map<String, Variant> change_;
};

bool YeelightUpdate::recognise(const MetaObject &message)
{
    Variant method;
    if (!message.lookup("method", &method)) return false;
    YEELIGHT_EXPECT(method.is<String>());
    return method.to<String>() == "props";
}

YeelightUpdate::YeelightUpdate(const MetaObject &message):
    YeelightResponse{new State{message}}
{}

YeelightUpdate::YeelightUpdate(const YeelightResponse &response)
{
    if (response.is<YeelightUpdate>()) {
        *this = response.as<YeelightUpdate>();
    }
}

const Map<String, Variant> &YeelightUpdate::change() const
{
    return me().change_;
}

Variant YeelightUpdate::change(const String &key) const
{
    return me().change_(key);
}

bool YeelightUpdate::hasPowerChanged() const
{
    return change().contains("power");
}

bool YeelightUpdate::newPower() const
{
    return change("power").to<bool>();
}

bool YeelightUpdate::hasColorTempChanged() const
{
    return change().contains("ct");
}

int YeelightUpdate::newColorTemp() const
{
    return change("ct").to<long>();
}

bool YeelightUpdate::hasBrightnessChanged() const
{
    return change().contains("bright");
}

int YeelightUpdate::newBrightness() const
{
    return change("bright").to<long>();
}

bool YeelightUpdate::hasColorChanged() const
{
    return change().contains("rgb");
}

Color YeelightUpdate::newColor() const
{
    return static_cast<std::uint32_t>(change("rgb").to<long>());
}

bool YeelightUpdate::hasHueChanged() const
{
    return change().contains("hue");
}

int YeelightUpdate::newHue() const
{
    return change("hue").to<long>();
}

bool YeelightUpdate::hasSatChanged() const
{
    return change().contains("sat");
}

int YeelightUpdate::newSat() const
{
    return change("sat").to<long>();
}

bool YeelightUpdate::hasColorModeChanged() const
{
    return change().contains("color_mode");
}

YeelightColorMode YeelightUpdate::newColorMode() const
{
    return static_cast<YeelightColorMode>(static_cast<int>(change("color_mode").to<long>()));
}

const YeelightUpdate::State &YeelightUpdate::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::yee
