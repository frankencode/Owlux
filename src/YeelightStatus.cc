#include <yee/YeelightStatus>
#include <yee/YeelightProtocolError>
#include <cc/Format>
#include <cc/Casefree>
#include <cc/Map>
#include <cc/Uri>
#include <cc/input>
#include <cc/str>
#include <cc/DEBUG>

namespace cc::yee {

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
        YEELIGHT_EXPECT(text.read<bool>(&power_));
    }

    void readBrightness()
    {
        String text;
        YEELIGHT_EXPECT(keyValueMap_.lookup("bright", &text));
        YEELIGHT_EXPECT(text.read<int>(&brightness_));
        YEELIGHT_EXPECT(0 <= brightness_ && brightness_ <= 100);
    }

    void readColorMode()
    {
        String text;
        if (!keyValueMap_.lookup("color_mode", &text)) return;
        int colorMode = 0;
        YEELIGHT_EXPECT(text.read<int>(&colorMode));
        YEELIGHT_EXPECT(+YeelightColorMode::Min <= colorMode && colorMode <= +YeelightColorMode::Max);
        colorMode_ = static_cast<YeelightColorMode>(colorMode);
    }

    void readColorTemp()
    {
        String text;
        if (!keyValueMap_.lookup("ct", &text)) return;
        YEELIGHT_EXPECT(text.read<int>(&colorTemp_));
        YEELIGHT_EXPECT(colorTemp_ > 0);
    }

    void readColor()
    {
        String text;
        if (!keyValueMap_.lookup("rgb", &text)) return;
        std::uint32_t rgb = 0;
        YEELIGHT_EXPECT(text.read<std::uint32_t>(&rgb));
        color_ = rgb;
    }

    void readHue()
    {
        String text;
        if (!keyValueMap_.lookup("hue", &text)) return;
        YEELIGHT_EXPECT(text.read<int>(&hue_));
        YEELIGHT_EXPECT(0 <= hue_ && hue_ <= 359);
    }

    void readSat()
    {
        String text;
        if (!keyValueMap_.lookup("sat", &text)) return;
        YEELIGHT_EXPECT(text.read<int>(&sat_));
        YEELIGHT_EXPECT(0 <= sat_ && sat_ <= 100);
    }

    String model() const
    {
        return keyValueMap_("model");
    }

    Version firmwareVersion() const
    {
        return keyValueMap_("fw_ver");
    }

    String name() const
    {
        return keyValueMap_("name");
    }

    String displayName() const
    {
        String s = keyValueMap_("name");
        if (s == "") s = "Light " + hex(id_);
        return s;
    }

    bool power() const
    {
        return power_;
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
            << "  power: " << power_ << "\n"
            << "  brightness: " << brightness_ << "\n"
            << "  color-mode: " << colorMode_ << "\n"
            << "  color-temp: " << colorTemp_ << "\n"
            << "  color: " << color_ << "\n"
            << "  hue: " << hue_ << "\n"
            << "  sat: " << sat_ << "\n"
            << "}";
    }

    Map<Casefree<String>, String> keyValueMap_;
    Set<String> supportedMethods_;

    SocketAddress address_;
    long refreshInterval_ { -1 };
    long id_ { -1 };
    bool power_ { false };
    int brightness_ { -1 };
    YeelightColorMode colorMode_ { YeelightColorMode::Undef };
    int colorTemp_ { -1 };
    Color color_;
    int hue_ { -1 };
    int sat_ { -1 };
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
    return me().power_;
}

int YeelightStatus::brightness() const
{
    return me().brightness_;
}

YeelightColorMode YeelightStatus::colorMode() const
{
    return me().colorMode_;
}

int YeelightStatus::colorTemp() const
{
    return me().colorTemp_;
}

Color YeelightStatus::color() const
{
    return me().color_;
}

int YeelightStatus::hue() const
{
    return me().hue_;
}

int YeelightStatus::sat() const
{
    return me().sat_;
}

String YeelightStatus::operator()(const String &key) const
{
    return me().keyValueMap_(key);
}

String YeelightStatus::toString() const
{
    return me().toString();
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
