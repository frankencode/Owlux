#include <owlux/YeelightEstablishColor>
#include <cc/Format>

namespace cc::owlux {

String YeelightEstablishColor::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[\"%%\",%%,%%]"
        "}\r\n"}
            << id_ << method_
            << "color" << (color_.value() & 0xFFFFFF) << brightness_;
}

} // namespace cc::owlux
