#include <yee/setup/YeelightSecretSetup>
#include <cc/AesBlockCipher>
#include <cc/CbcBlockCipher>
#include <cc/BlockCipherSink>
#include <cc/CaptureSink>
#include <cc/ModuloPadding>
#include <cc/System>
#include <cc/Format>
#include <cc/md5>

namespace cc::yee {

struct YeelightSecretSetup::State: public YeelightSecretMessage::State
{
    State(
        const YeelightSecretStatus &status,
        const Bytes &token,
        const String &networkName,
        const String &networkPassword
    ) {
        String clearText {
            Format{
                "{\"id\":%%,"
                "\"method\":\"miIO.config_router\","
                "\"params\":{\"ssid\":\"%%\",\"passwd\":\"%%\"}}"
            }
                << uint64_t(System::now())
                << networkName
                << networkPassword
        };

        CaptureSink capture;
        {
            Bytes key = md5(token);
            Bytes start = md5(Bytes{key, token});

            CbcBlockCipher cipher{AesBlockCipher{key}, start};
            BlockCipherSink sink{capture, cipher};
            sink.write(clearText);
            ModuloPadding::writePadding(&sink);
        }

        payload_ = Bytes{capture.collect()};
        message_ = Bytes::allocate(32 + payload_.count());
        message_.fill(0);
        status.message().copyRangeTo(0, 32, &message_);
        length_ = message_.count();
        message_.at(2) = (length_ >> 8) & 0xFF;
        message_.at(3) = length_ & 0xFF;
        payload_.copyToOffset(&message_, 32);

        checksum_ = computeChecksum(token);
        checksum_.copyToOffset(&message_, 16);
    }
};

YeelightSecretSetup::YeelightSecretSetup(
    const YeelightSecretStatus &status,
    const Bytes &token,
    const String &networkName,
    const String &networkPassword
):
    YeelightSecretMessage{new State{status, token, networkName, networkPassword}}
{}

} // namespace cc::yee
