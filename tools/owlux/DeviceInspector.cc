/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/DeviceInspector>
#include <cc/StackView>
#include <cc/AppBar>
#include <cc/LogView>

namespace cc::owlux {

struct DeviceInspector::State final: public View::State
{
    State(const YeelightStatus &status, const StackView &stack):
        stack_{stack}
    {
        add(
            AppBar{}
            .associate(&appBar_)
            .title("Device Information")
            .onDismissed([this] {
                stack_.pop();
            })
        );

        add(
            LogView{}
            .associate(&logView_)
            .pos(0, appBar_.height())
            .size([this]{ return size() - Size{0, appBar_.height()}; })
            .add(FlickableIndicator{})
        );

        logView_.appendText(status.toString());
    }

    StackView stack_;
    AppBar appBar_;
    LogView logView_;
};

DeviceInspector::DeviceInspector(const YeelightStatus &status, const StackView &stack):
    View{new State{status, stack}}
{}

DeviceInspector::State &DeviceInspector::me()
{
    return get<State>();
}

const DeviceInspector::State &DeviceInspector::me() const
{
    return get<State>();
}

} // namespace cc::owlux
