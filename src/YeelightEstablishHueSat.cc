#include <owlux/YeelightEstablishHueSat>
#include <cc/Format>

namespace cc::owlux {

String YeelightEstablishHueSat::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[\"%%\",%%,%%,%%]"
        "}\r\n"}
            << id_ << method_
            << "hsv" << hue_ << sat_ << brightness_;
}

} // namespace cc::owlux
