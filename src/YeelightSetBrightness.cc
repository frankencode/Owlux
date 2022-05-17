#include <yee/YeelightSetBrightness>
#include <cc/Format>

namespace cc::yee {

String YeelightSetBrightness::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,"
            "\"method\":\"%%\","
            "\"params\":[%%,\"%%\",%%]"
        "}\r\n"}
            << id_ << method_
            << brightness_
            << yeeStr(effect_)
            << duration_;
}

} // namespace cc::yee
