#include <beelight/DiscoveryView>
#include <beelight/ColorController>
#include <cc/StackView>
#include <cc/DEBUG>

int main()
{
    using namespace cc::yee;

    StackView stack;
    DiscoveryView discovery;

    return
        StackView{sp(480), sp(800)}
        .associate(&stack)
        .push(
            DiscoveryView{}
            .associate(&discovery)
            .onSelected([=](const YeelightStatus &status) mutable {
                stack.push(
                    ColorController{status}
                    .onDismissed([=]() mutable {
                        stack.pop();
                    })
                );
                // CC_INSPECT(status.address());
            })
        )
        .run();
}
