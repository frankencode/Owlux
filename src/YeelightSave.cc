#include <owlux/YeelightSave>
#include <cc/Format>

namespace cc::owlux {

String YeelightSave::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,"
            "\"method\":\"%%\","
            "\"params\":[]"
        "}\r\n"}
            << id_ << method_;
}

} // namespace cc::owlux
