#include <yee/YeelightSetName>
#include <cc/Format>

namespace cc::yee {

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

} // namespace cc::yee
