#include <owlux/YeelightStartOffTimer>
#include <owlux/YeelightPower>
#include <cc/System>
#include <cc/Format>

namespace cc::owlux {

struct YeelightStartOffTimer::State final: public YeelightCommand::State
{
    explicit State(int delayInMinutes):
        YeelightCommand::State{"cron_add"},
        delay_{delayInMinutes}
    {}

    String toString() const
    {
        if (delay_ <= 0) return YeelightPower{false}.toString();

        return
            Format{"{"
                "\"id\":%%,\"method\":\"%%\","
                "\"params\":[0,%%]"
            "}\r\n"}
                << id_ << method_
                << delay_;
    }

    int delay_;
};

YeelightStartOffTimer::YeelightStartOffTimer(int delayInMinutes):
    YeelightCommand{new State{delayInMinutes}}
{}

} // namespace cc::owlux
