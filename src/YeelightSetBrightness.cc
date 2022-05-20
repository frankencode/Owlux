#include <owlux/YeelightSetBrightness>
#include <cc/Format>

namespace cc::owlux {

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

} // namespace cc::owlux
