#include <owlux/YeelightPower>
#include <cc/Format>

namespace cc::owlux {

String YeelightPower::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,"
            "\"method\":\"%%\","
            "\"params\":[\"%%\",\"%%\",%%,%%]"
        "}\r\n"}
            << id_ << method_
            << (on_ ? "on" : "off")
            << yeeStr(effect_)
            << duration_
            << +colorMode_;
}

} // namespace cc::owlux
