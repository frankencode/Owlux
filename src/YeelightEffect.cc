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
