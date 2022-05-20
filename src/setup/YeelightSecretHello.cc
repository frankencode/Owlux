#include <owlux/setup/YeelightSecretHello>

namespace cc::owlux {

Bytes YeelightSecretHello::produce()
{
    Bytes message = Bytes::allocate(32);
    message.fill(0xFF);
    Bytes{0x21, 0x31, 0x00, 0x20}.copyTo(&message);
    return message;
}

} // namespace cc::owlux
