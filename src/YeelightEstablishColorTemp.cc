#include <owlux/YeelightEstablishColorTemp>
#include <cc/Format>

namespace cc::owlux {

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

} // namespace cc::owlux
