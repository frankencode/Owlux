/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

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
