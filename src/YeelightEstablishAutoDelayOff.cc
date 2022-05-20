#include <owlux/YeelightEstablishAutoDelayOff>
#include <cc/Format>

namespace cc::owlux {

String YeelightEstablishAutoDelayOff::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[\"%%\",%%,%%]"
        "}\r\n"}
            << id_ << method_
            << "auto_delay_off" << brightness_ << delay_;
}

} // namespae cc::owlux
