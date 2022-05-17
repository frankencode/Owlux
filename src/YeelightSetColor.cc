#include <yee/YeelightSetColor>
#include <cc/Format>

namespace cc::yee {

String YeelightSetColor::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[%%,\"%%\",%%]"
        "}\r\n"}
            << id_ << method_
            << (+color_ & 0xFFFFFFu) << yeeStr(effect_) << duration_;
}

} // namespace cc::yee
