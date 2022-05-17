#include <yee/YeelightSave>
#include <cc/Format>

namespace cc::yee {

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

} // namespace cc::yee
