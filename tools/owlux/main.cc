#include <owlux/DiscoveryView>
#include <owlux/LightSettings>
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
            StackView{sp(340), sp(640)}
            .associate(&stack)
            .push(
                DiscoveryView{}
                .associate(&discovery)
                .onSelected([=](const YeelightStatus &status) mutable {
                    stack.push(
                        LightSettings{status, stack}
                    );
                })
            )
        }
        .title("Owlux")
        .icon(":/icons/app.webp")
        .run();
}
