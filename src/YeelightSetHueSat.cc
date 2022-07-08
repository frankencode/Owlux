/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightSetHueSat>
#include <cc/Format>

namespace cc::owlux {

String YeelightSetHueSat::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[%%,%%,\"%%\",%%]"
        "}\r\n"}
            << id_ << method_
            << hue_ << sat_ << yeeStr(effect_) << duration_;
}

} // namespace cc::owlux
