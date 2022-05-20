#include <owlux/YeelightStopColorFlow>
#include <cc/Format>

namespace cc::owlux {

String YeelightStopColorFlow::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[]"
        "}\r\n"}
            << id_ << method_;
}

} // namespace cc::owlux
