#include <yee/YeelightEstablishColorFlow>
#include <cc/Format>

namespace cc::yee {

String YeelightEstablishColorFlow::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[\"%%\",%%,%%,\"%%\"]"
        "}\r\n"}
            << id_ << method_
            << "cf"
            << ((flow_.closure() == YeelightColorFlow::Closure::Repeat) ? 0 : flow_.transitions().count())
            << ((flow_.closure() == YeelightColorFlow::Closure::Repeat) ? 0 : static_cast<int>(flow_.closure()))
            << flow_.toString();
}

} // namespace cc::yee
