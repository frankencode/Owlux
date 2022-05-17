#include <yee/YeelightEstablishColorTemp>
#include <cc/Format>

namespace cc::yee {

String YeelightEstablishColorTemp::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[\"%%\",%%,%%]"
        "}\r\n"}
            << id_ << method_
            << "ct" << colorTemp_ << brightness_;
}

} // namespace cc::yee
