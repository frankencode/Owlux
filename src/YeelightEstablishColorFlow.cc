/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightEstablishColorFlow>
#include <cc/Format>

namespace cc::owlux {

String YeelightEstablishColorFlow::State::toString() const
{
    return
        Format{"{"
            "\"id\":%%,\"method\":\"%%\","
            "\"params\":[\"%%\",%%,%%,\"%%\"]"
        "}\r\n"}
            << id_ << method_
            << "cf"
            << ((flow_.closure() == YeelightColorFlow::Closure::Repeat) ? 0 : flow_.transitions().count())
            << ((flow_.closure() == YeelightColorFlow::Closure::Repeat) ? 0 : static_cast<int>(flow_.closure()))
            << flow_.toString();
}

} // namespace cc::owlux
