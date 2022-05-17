#include <yee/YeelightColorMode>

namespace cc::yee {

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

} // namespace cc::yee
