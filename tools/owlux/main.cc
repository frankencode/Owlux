#include <owlux/DiscoveryView>
#include <owlux/ColorController>
#include <cc/StackView>
#include <cc/Window>
#include <cc/Resource>

CC_RESOURCE("icons")

int main()
{
    using namespace cc::owlux;

    StackView stack;
    DiscoveryView discovery;

    return
        Window{
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
                })
            )
        }
        .icon(":/icons/app.webp")
        .run();
}
