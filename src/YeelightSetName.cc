#include <owlux/YeelightSetName>
#include <cc/Format>

namespace cc::owlux {

String YeelightSetName::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,"
            "\"method\":\"%%\","
            "\"params\":[\"%%\"]"
        "}\r\n"}
            << id_ << method_
            << name_;
}

} // namespace cc::owlux
