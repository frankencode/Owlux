#include <yee/setup/YeelightSecretResult>
#include <cc/json>

namespace cc::yee {

YeelightSecretResult::YeelightSecretResult(const Bytes &message):
    YeelightSecretMessage{message}
{}

YeelightResult YeelightSecretResult::decipherResult(const Bytes &token)
{
    String line = decipherPayload(token);
    Variant messageValue = jsonParse(line);
    YEELIGHT_EXPECT(messageValue.is<MetaObject>());
    return YeelightResult{messageValue.to<MetaObject>()};
}

} // namespace cc::yee
