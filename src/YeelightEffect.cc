/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightEffect>

namespace cc::owlux {

String yeeStr(YeelightEffect effect)
{
    static String sudden { "sudden" };
    static String smooth { "smooth" };
    String s;
    switch(effect) {
        case YeelightEffect::Sudden: s = sudden; break;
        case YeelightEffect::Smooth: s = smooth; break;
    }
    return s;
}

} // namespace cc::owlux
