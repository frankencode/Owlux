#include <owlux/YeelightStopOffTimer>
#include <cc/Format>

namespace cc::owlux {

struct YeelightStopOffTimer::State final: public YeelightCommand::State
{
    explicit State():
        YeelightCommand::State{"cron_del"}
    {}

    String toString() const
    {
        return
            Format{"{"
                "\"id\":%%,\"method\":\"%%\","
                "\"params\":[0]"
            "}\r\n"}
                << id_ << method_;
    }
};

YeelightStopOffTimer::YeelightStopOffTimer():
    YeelightCommand{new State}
{}

} // namespace cc::owlux
