/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightColorFlow>
#include <owlux/YeelightProtocolError>
#include <owlux/YeelightColorTransition>
#include <owlux/YeelightColorTempTransition>
#include <owlux/YeelightSleepTransition>
#include <cc/Format>

namespace cc::owlux {

YeelightColorFlow::State::State(const String &expression)
{
    List<String> parts = expression.split(',');
    if (parts.count() > 0) {
        if (parts.last() == "repeat") {
            closure_ = Closure::Repeat;
            parts.popBack();
        }
        else if (parts.last() == "return") {
            closure_ = Closure::Return;
            parts.popBack();
        }
        else if (parts.last() == "stay") {
            closure_ = Closure::Stay;
            parts.popBack();
        }
        else if (parts.last() == "off") {
            closure_ = Closure::Off;
            parts.popBack();
        }
    }
    YEELIGHT_EXPECT(parts.count() % 4 == 0);
    YEELIGHT_EXPECT(parts.count() > 0);
    YeelightTransition::Tuple tuple;
    int j = 0;
    for (const String &part: parts) {
        bool isInteger = false;
        tuple[j] = part.toInt(&isInteger);
        YEELIGHT_EXPECT(isInteger);
        ++j;
        if (j == 4) {
            j = 0;
            YeelightTransition::Type type = static_cast<YeelightTransition::Type>(tuple[1]);
            switch (type) {
                case YeelightTransition::Type::ColorChange: {
                    transitions_.append(
                        YeelightColorTransition{tuple[0], Color{static_cast<uint32_t>(tuple[2])}, tuple[3]}
                    );
                    break;
                }
                case YeelightTransition::Type::ColorTempChange: {
                    transitions_.append(
                        YeelightColorTempTransition{tuple[0], tuple[2], tuple[3]}
                    );
                    break;
                }
                case YeelightTransition::Type::Sleep: {
                    transitions_.append(
                        YeelightSleepTransition{tuple[0]}
                    );
                    break;
                }
                default: {
                    YEELIGHT_EXPECT(false);
                }
            };
        }
    }
}

String YeelightColorFlow::State::toString() const
{
    Format f;
    bool firstTime = true;
    for (const YeelightTransition &transition: transitions_) {
        auto tuple = transition.toTuple();
        if (firstTime) firstTime = false;
        else f << ",";
        f << tuple[0] << "," << tuple[1] << "," << tuple[2] << "," << tuple[3];
    }
    return f;
}

YeelightColorFlow YeelightColorFlow::alarm(const Color &color, int interval)
{
    if (interval < 200) interval = 200;
    int h = interval - 100;
    int c = color.value() & 0xFFFFFF;
    return YeelightColorFlow{
        Format{"50,1,%%,1,%%,7,0,0,50,1,%%,100,%%,7,0,0,repeat"} << c << h << c << h
    };
}

} // namespace cc::owlux
