#include <yee/setup/YeelightSecretMessage>
#include <cc/ByteSource>
#include <cc/MemoryStream>
#include <cc/CbcBlockCipher>
#include <cc/AesBlockCipher>
#include <cc/BlockCipherSource>
#include <cc/ModuloPadding>
#include <cc/md5>
#include <cc/HexDump> // DEBUG
#include <cc/DEBUG>

namespace cc::yee {

YeelightSecretMessage::State::State(const Bytes &message):
    message_{message}
{
    YEELIGHT_EXPECT(message.count() >= 32);
    {
        ByteSource source{message, ByteOrder::BigEndian};
        magic_ = source.readUInt16();
        YEELIGHT_EXPECT(magic_ == 0x2131);
        length_ = source.readUInt16();
        deviceId_ = source.readUInt64();
        uptime_ = source.readUInt32();
    }
    checksum_ = message_.select(16, 32);
    if (message.count() > 32) {
        payload_ = message_.select(32, message_.count());
    }
}

Bytes YeelightSecretMessage::State::computeChecksum(const Bytes &token) const
{
    Bytes message = message_.copy();
    token.copyToOffset(&message, 16);
    return md5(message);
}

String YeelightSecretMessage::State::deciperPayload(const Bytes &token) const
{
    Bytes key = md5(token);
    Bytes start = md5(Bytes{key, token});

    CbcBlockCipher cipher{AesBlockCipher{key}, start};
    BlockCipherSource source{MemoryStream{payload_}, cipher};

    String message = source.readAll();
    ModuloPadding::removePadding(&message);
    return message;
}

YeelightSecretMessage::YeelightSecretMessage(const Bytes &message):
    Object{new State{message}}
{}

YeelightSecretMessage::YeelightSecretMessage(State *newState):
    Object{newState}
{}

} // namespace cc::yee
