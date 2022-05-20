#include <owlux/YeelightSetHueSat>
#include <cc/Format>

namespace cc::owlux {

String YeelightSetHueSat::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[%%,%%,\"%%\",%%]"
        "}\r\n"}
            << id_ << method_
            << hue_ << sat_ << yeeStr(effect_) << duration_;
}

} // namespace cc::owlux
