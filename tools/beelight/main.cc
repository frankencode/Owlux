#include <beelight/DiscoveryView>
#include <cc/StackView>

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
        )
        .run();
}
