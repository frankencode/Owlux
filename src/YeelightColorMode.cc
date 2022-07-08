/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightColorMode>

namespace cc::owlux {

String str(YeelightColorMode mode)
{
    String s;
    switch (mode) {
        case YeelightColorMode::Undef       : s = "YeelightColorMode::Undef"; break;
        case YeelightColorMode::Default     : s = "YeelightColorMode::Default"; break;
        case YeelightColorMode::ColorTemp   : s = "YeelightColorMode::ColorTemp"; break;
        case YeelightColorMode::RedGreenBlue: s = "YeelightColorMode::RedGreenBlue"; break;
        case YeelightColorMode::HueSat      : s = "YeelightColorMode::HueSat"; break;
        case YeelightColorMode::ColorFlow   : s = "YeelightColorMode::ColorFlow"; break;
        case YeelightColorMode::NightLight  : s = "YeelightColorMode::NightLight"; break;
    };
    return s;
}

} // namespace cc::owlux
