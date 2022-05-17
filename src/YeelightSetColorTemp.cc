#include <yee/YeelightSetColorTemp>
#include <cc/Format>

namespace cc::yee {

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

} // namespace cc::yee
