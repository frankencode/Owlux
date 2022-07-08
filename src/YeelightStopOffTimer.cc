/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

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
