#include <owlux/YeelightStatus>
#include <owlux/YeelightProtocolError>
#include <owlux/YeelightUpdate>
#include <cc/Format>
#include <cc/Casefree>
#include <cc/Map>
#include <cc/Uri>
#include <cc/Property>
#include <cc/input>
#include <cc/str>
#include <cc/DEBUG>
#include <cmath>

namespace cc::owlux {

struct YeelightStatus::State final: public Object::State
{
public:
    State(const SocketAddress &address, const String &message):
        address_{address}
    {
        List<String> lines = message.split("\r\n");
        YEELIGHT_EXPECT(lines.count() >= 2);
        readStatus(lines.first());
        lines.popFront();

        for (const String &line: lines) readKeyValue(line);

        readName();
        readCacheControl();
        readLocation();
        readId();

        for (const String &method: keyValueMap_("support").split(' ')) {
            supportedMethods_.insert(method);
        }

        readPower();
        readBrightness();
        readColorMode();
        readColorTemp();
        readColor();
        readHue();
        readSat();
    }

    void readStatus(const String &line)
    {
        const char *expectedStatus = "HTTP/1.1 200 OK";
        const char *expectedAnnouncement = "NOTIFY * HTTP/1.1";
        YEELIGHT_EXPECT(line == expectedStatus || line == expectedAnnouncement);
    }

    void readKeyValue(const String &line)
    {
        if (line == "") return;
        long i = 0;
        YEELIGHT_EXPECT(line.find(':', &i));
        String key = line.copy(0, i);
        ++i;
        for (; i < line.count(); ++i) {
            if (line.at(i) > ' ') break;
        }
        String value = line.copy(i, line.count());
        if (value != "") {
            keyValueMap_.insert(key, value);
        }
    }

    void readName()
    {
        name = keyValueMap_.value("name");
    }

    void readCacheControl()
    {
        String text;
        if (!keyValueMap_.lookup("Cache-Control", &text)) return;

        List<String> parts = text.split('=');
        YEELIGHT_EXPECT(parts.count() == 2);

        String key = parts.at(0);
        String value = parts.at(1);
        if (key == "max-age") {
            YEELIGHT_EXPECT(value.read<long>(&refreshInterval_));
        }
    }

    void readLocation()
    {
        String text;
        if (!keyValueMap_.lookup("Location", &text)) return;

        Uri uri{text};
        YEELIGHT_EXPECT(uri.scheme() == "yeelight");

        address_.setPort(uri.port());
    }

    void readId()
    {
        String text;
        YEELIGHT_EXPECT(keyValueMap_.lookup("id", &text));
        YEELIGHT_EXPECT(text.read<long>(&id_));
    }

    void readPower()
    {
        String text;
        YEELIGHT_EXPECT(keyValueMap_.lookup("power", &text));
        YEELIGHT_EXPECT(power.readFrom(text));
    }

    void readBrightness()
    {
        String text;
        YEELIGHT_EXPECT(keyValueMap_.lookup("bright", &text));
        YEELIGHT_EXPECT(brightness.readFrom(text));
        YEELIGHT_EXPECT(0 <= brightness() && brightness() <= 100);
    }

    void readColorMode()
    {
        String text;
        if (!keyValueMap_.lookup("color_mode", &text)) return;
        int mode = 0;
        YEELIGHT_EXPECT(text.read<int>(&mode));
        YEELIGHT_EXPECT(+YeelightColorMode::Min <= mode && mode <= +YeelightColorMode::Max);
        colorMode = static_cast<YeelightColorMode>(mode);
    }

    void readColorTemp()
    {
        String text;
        if (!keyValueMap_.lookup("ct", &text)) return;
        YEELIGHT_EXPECT(colorTemp.readFrom(text));
        YEELIGHT_EXPECT(colorTemp() > 0);
    }

    void readColor()
    {
        String text;
        if (!keyValueMap_.lookup("rgb", &text)) return;
        std::uint32_t rgb = 0;
        YEELIGHT_EXPECT(text.read<std::uint32_t>(&rgb));
        color = rgb;
    }

    void readHue()
    {
        String text;
        if (!keyValueMap_.lookup("hue", &text)) return;
        YEELIGHT_EXPECT(hue.readFrom(text));
        YEELIGHT_EXPECT(0 <= hue() && hue() <= 359);
    }

    void readSat()
    {
        String text;
        if (!keyValueMap_.lookup("sat", &text)) return;
        YEELIGHT_EXPECT(sat.readFrom(text));
        YEELIGHT_EXPECT(0 <= sat() && sat() <= 100);
    }

    String model() const
    {
        return keyValueMap_("model");
    }

    Version firmwareVersion() const
    {
        return keyValueMap_("fw_ver");
    }

    String displayName() const
    {
        String s = name();
        if (s == "") s = "Light " + hex(id_);
        return s;
    }

    String toString() const
    {
        return Format{}
            << "YeelightStatus {\n"
            << "  id: 0x" << hex(id_) << "\n"
            << "  name: " << name() << "\n"
            << "  model: " << model() << "\n"
            << "  address: " << address_.toString() << "\n"
            << "  refresh-interval: " << refreshInterval_ << "\n"
            << "  supported-methods: " << supportedMethods_ << "\n"
            << "  firmware-version: " << firmwareVersion() << "\n"
            << "  power: " << power() << "\n"
            << "  brightness: " << brightness() << "\n"
            << "  color-mode: " << colorMode() << "\n"
            << "  color-temp: " << colorTemp() << "\n"
            << "  color: " << color() << "\n"
            << "  hue: " << hue() << "\n"
            << "  sat: " << sat() << "\n"
            << "}";
    }

    void update(const YeelightUpdate &update)
    {
        if (update.hasNameChanged()) name = update.newName();
        if (update.hasPowerChanged()) power = update.newPower();
        if (update.hasBrightnessChanged()) brightness = update.newBrightness();
        if (update.hasColorModeChanged()) colorMode = update.newColorMode();
        if (update.hasColorTempChanged()) colorTemp = update.newColorTemp();
        if (update.hasColorChanged()) color = update.newColor();
        if (update.hasHueChanged()) hue = update.newHue();
        if (update.hasSatChanged()) sat = update.newSat();
    }

    void update(const YeelightStatus &update)
    {
        name = update.name();
        power = update.power();
        brightness = update.brightness();
        colorMode = update.colorMode();
        colorTemp = update.colorTemp();
        color = update.color();
        hue = update.hue();
        sat = update.sat();
    }

    Map<Casefree<String>, String> keyValueMap_;
    Set<String> supportedMethods_;

    SocketAddress address_;
    long refreshInterval_ { -1 };
    long id_ { -1 };
    Property<String> name;
    Property<bool> power { false };
    Property<int> brightness { -1 };
    Property<YeelightColorMode> colorMode { YeelightColorMode::Undef };
    Property<int> colorTemp { -1 };
    Property<Color> color;
    Property<int> hue { -1 };
    Property<int> sat { -1 };
    Property<bool> sleepTimer;
    Property<int> sleepHour;
    Property<int> sleepMinutes;
    Property<double> offTime { std::numeric_limits<double>::quiet_NaN() };
    Property<Date> offDate { [this]{ return std::isnan(offTime()) ? Date{} : Date::local(offTime()); } };
};

YeelightStatus::YeelightStatus(const SocketAddress &address, const String &message):
    Object{new State{address, message}}
{}

SocketAddress YeelightStatus::address() const
{
    return me().address_;
}

long YeelightStatus::refreshInterval() const
{
    return me().refreshInterval_;
}

long YeelightStatus::id() const
{
    return me().id_;
}

String YeelightStatus::name() const
{
    return me().name();
}

void YeelightStatus::setName(const String &newValue)
{
    me().name(newValue);
}

String YeelightStatus::displayName() const
{
    return me().displayName();
}

String YeelightStatus::model() const
{
    return me().model();
}

const Set<String> &YeelightStatus::supportedMethods() const
{
    return me().supportedMethods_;
}

Version YeelightStatus::firmwareVersion() const
{
    return me().firmwareVersion();
}

bool YeelightStatus::power() const
{
    return me().power();
}

void YeelightStatus::setPower(bool on)
{
    me().power(on);
}

int YeelightStatus::brightness() const
{
    return me().brightness();
}

YeelightColorMode YeelightStatus::colorMode() const
{
    return me().colorMode();
}

int YeelightStatus::colorTemp() const
{
    return me().colorTemp();
}

Color YeelightStatus::color() const
{
    return me().color();
}

int YeelightStatus::hue() const
{
    return me().hue();
}

int YeelightStatus::sat() const
{
    return me().sat();
}

void YeelightStatus::setSat(int newValue)
{
    me().sat(newValue);
}

String YeelightStatus::operator()(const String &key) const
{
    return me().keyValueMap_(key);
}

bool YeelightStatus::hasOffTime() const
{
    return !std::isnan(me().offTime());
}

double YeelightStatus::offTime() const
{
    return me().offTime();
}

Date YeelightStatus::offDate() const
{
    return me().offDate();
}

void YeelightStatus::setOffTime(double newValue)
{
    me().offTime(newValue);
}

void YeelightStatus::unsetOffTime()
{
    me().offTime(std::numeric_limits<double>::quiet_NaN());
}

String YeelightStatus::toString() const
{
    return me().toString();
}

void YeelightStatus::update(const YeelightUpdate &update)
{
    me().update(update);
}

void YeelightStatus::update(const YeelightStatus &update)
{
    me().update(update);
}

YeelightStatus::State &YeelightStatus::me()
{
    return Object::me.as<State>();
}

const YeelightStatus::State &YeelightStatus::me() const
{
    return Object::me.as<State>();
}

} // name
