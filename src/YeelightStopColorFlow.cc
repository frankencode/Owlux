/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

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
