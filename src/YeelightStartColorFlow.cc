#include <owlux/YeelightStartColorFlow>
#include <cc/Format>

namespace cc::owlux {

String YeelightStartColorFlow::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[%%,%%,\"%%\"]"
        "}\r\n"}
            << id_ << method_
            << ((flow_.closure() == YeelightColorFlow::Closure::Repeat) ? 0 : flow_.transitions().count())
            << ((flow_.closure() == YeelightColorFlow::Closure::Repeat) ? 0 : static_cast<int>(flow_.closure()))
            << flow_.toString();
}

} // namespace cc::owlux
