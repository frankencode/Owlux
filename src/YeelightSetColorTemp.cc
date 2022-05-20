#include <owlux/YeelightSetColorTemp>
#include <cc/Format>

namespace cc::owlux {

String YeelightSetColorTemp::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,"
            "\"method\":\"%%\","
            "\"params\":[%%,\"%%\",%%]"
        "}\r\n"}
        << id_<< method_
        << colorTemp_ << yeeStr(effect_) << duration_;
}

} // namespace cc::owlux
