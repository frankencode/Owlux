#include <yee/setup/YeelightSecretHello>
#include <yee/setup/YeelightSecretStatus>
#include <yee/setup/YeelightSecretSetup>
#include <yee/setup/YeelightSecretResult>
#include <yee/setup/YeelightSecretMessage>
#include <cc/AesBlockCipher>
#include <cc/CbcBlockCipher>
#include <cc/BlockCipherSource>
#include <cc/MemoryStream>
#include <cc/ModuloPadding>
#include <cc/System>
#include <cc/HexDump>
#include <cc/md5>
#include <cc/testing>
#include <cc/DEBUG>

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::yee;

    #if 0
    CC_INSPECT(fixed(System::now(), 0));

    //// Yeelight Color 1s 06/2020
    {
        Bytes cipherText {
            0x6d, 0x24, 0x32, 0x5a, 0x39, 0x77, 0xbb, 0xdf, 0x43, 0x90, 0x95, 0xb1, 0x22, 0xa2, 0xe0, 0x69,
            0x1f, 0x24, 0x7e, 0x0c, 0x00, 0x91, 0x9c, 0xb1, 0x6b, 0x27, 0x68, 0x6a, 0x0a, 0x70, 0xb6, 0xd7,
            0x6c, 0xce, 0x0e, 0x11, 0x49, 0x93, 0x10, 0xa7, 0xb7, 0xd8, 0x35, 0x20, 0x76, 0x8b, 0x86, 0x52,
            0xe3, 0x48, 0xce, 0xee, 0xfa, 0x43, 0xcd, 0x41, 0xbe, 0x50, 0x5a, 0x28, 0x07, 0x2d, 0xa4, 0x6c,
            0x96, 0xf4, 0x60, 0xf8, 0xcf, 0xe6, 0x79, 0x42, 0xe2, 0x8a, 0x4a, 0xba, 0x52, 0x8f, 0x3a, 0x03,
            0x4c, 0xcc, 0xaa, 0x07, 0xf5, 0x3c, 0x97, 0x9b, 0x53, 0x99, 0x0e, 0xbc, 0x52, 0xe3, 0xea, 0x6f,
            0x07, 0x81, 0xc8, 0xe2, 0x9b, 0xdd, 0xf9, 0x70, 0xdf, 0x39, 0x51, 0x90, 0xaf, 0xa6, 0x4a, 0x01,
            0x2e, 0xe2, 0xd8, 0x9e, 0x58, 0xf2, 0xf7, 0xbe, 0x02, 0xb1, 0xe7, 0x5a, 0x1e, 0xf7, 0x6b, 0x4b,
            0x08, 0xc6, 0x72, 0x83, 0xf1, 0x91, 0xa4, 0xcd, 0x6d, 0xc0, 0xb2, 0x0c, 0xa2, 0x45, 0x55, 0xce,
            0xdf, 0xc0, 0xbf, 0x5b, 0xa3, 0x10, 0xab, 0xa1, 0xf3, 0x45, 0x6a, 0x1c, 0x65, 0x1e, 0xb7, 0xd8,
            0xe8, 0x17, 0x47, 0xdc, 0x4c, 0x8b, 0xda, 0x06, 0xbe, 0xcb, 0xb4, 0x15, 0xb3, 0x1a, 0x82, 0x12,
            0xcf, 0xbf, 0xee, 0x36, 0xbd, 0xc7, 0xbe, 0x91, 0xcd, 0xb0, 0x80, 0x14, 0x45, 0xba, 0x58, 0x8b,
            0xb5, 0xbf, 0x3b, 0x59, 0x61, 0x68, 0x99, 0xe1, 0xf2, 0xd4, 0x21, 0xf0, 0x90, 0xba, 0x26, 0x2a,
            0xb3, 0x03, 0x03, 0x92, 0x71, 0x82, 0xc9, 0x68, 0xa7, 0xec, 0x97, 0xfd, 0x22, 0x24, 0xe6, 0xc2
        };

        Bytes token {
            0x53, 0x00, 0x87, 0x61, 0x55, 0xf0, 0x8d, 0x0a, 0x9f, 0x1e, 0x10, 0xa0, 0xe1, 0xa2, 0x1e, 0x89
        };

        Bytes key = md5(token);
        Bytes start = md5(Bytes{key, token});

        CC_INSPECT(hex(key));
        CC_INSPECT(hex(start));

        CbcBlockCipher cipher{AesBlockCipher{key}, start};
        BlockCipherSource source{MemoryStream{cipherText}, cipher};

        String message = source.readAll();
        ModuloPadding::removePadding(&message);
        CC_INSPECT(message);
    }

    //// Yeelight Color Bulb 04:91 07/2018 (original firmware)
    {
        Bytes cipherText {
            0x66, 0x81, 0xbc, 0xb7, 0x43, 0x21, 0x34, 0x71, 0x03, 0xcb, 0xeb, 0x0d, 0xe2, 0x37, 0xe0, 0x78,
            0x33, 0x47, 0x89, 0xfc, 0xe8, 0xe4, 0x3f, 0x50, 0x8c, 0xfe, 0x19, 0x7b, 0x2b, 0x69, 0x26, 0x77,
            0xb1, 0x1c, 0x44, 0x25, 0x40, 0x88, 0x0f, 0x86, 0x97, 0x87, 0x58, 0xf1, 0xec, 0x0e, 0x29, 0xec,
            0x84, 0xdd, 0x97, 0x84, 0xcc, 0xe0, 0xb2, 0xb8, 0xc5, 0x7f, 0x7c, 0xd4, 0x66, 0x4d, 0x8c, 0x35,
            0x83, 0x41, 0x74, 0x0a, 0xe2, 0xa8, 0x53, 0x7b, 0x23, 0xc0, 0x86, 0x50, 0x05, 0xee, 0xef, 0xb2,
            0x48, 0x1d, 0x10, 0x83, 0x31, 0x31, 0x98, 0x55, 0xb0, 0x5a, 0x95, 0xf4, 0xc0, 0x66, 0xde, 0x16,
            0xbb, 0xb2, 0xea, 0xcf, 0x26, 0xbe, 0xb2, 0xcb, 0xb7, 0x9c, 0xa2, 0x6b, 0xe2, 0x3b, 0x52, 0x96,
            0x26, 0xf9, 0x0f, 0x8f, 0x9a, 0xf3, 0xbf, 0x94, 0x62, 0xbe, 0xb4, 0x83, 0x90, 0xcd, 0xe8, 0xa8,
            0x67, 0xfc, 0xf0, 0x56, 0x66, 0xcf, 0xca, 0x6f, 0xe9, 0x96, 0xc4, 0xb8, 0x34, 0xcd, 0x4d, 0x6d,
            0x58, 0x72, 0xc1, 0x38, 0xd9, 0x35, 0xfc, 0x2f, 0x53, 0x97, 0x80, 0xb6, 0x06, 0xee, 0x0c, 0x36,
            0xa1, 0x92, 0x70, 0xce, 0x72, 0xec, 0x4d, 0x75, 0x0b, 0xdb, 0x35, 0xb9, 0x75, 0x36, 0xf2, 0x9f,
            0x9a, 0x74, 0x2c, 0x51, 0x54, 0xa3, 0x5f, 0xae, 0x8c, 0xa3, 0x25, 0xd4, 0xe7, 0x73, 0x98, 0x7f,
            0x23, 0xd5, 0xea, 0x28, 0xd3, 0xa8, 0x5d, 0xd2, 0xa0, 0xaf, 0x7d, 0xaa, 0xb0, 0xee, 0xb3, 0x1f,
            0x16, 0x1c, 0x89, 0x89, 0x20, 0x56, 0x4e, 0x56, 0x86, 0xe9, 0xee, 0xab, 0xc1, 0xde, 0x02, 0x70
        };

        Bytes token {
            0x26, 0xae, 0xd3, 0x07, 0xfb, 0x4f, 0x0e, 0x4c, 0xf3, 0x6a, 0x1f, 0x85, 0xfd, 0xf8, 0x78, 0x93
        };

        Bytes key = md5(token);
        Bytes start = md5(Bytes{key, token});

        CC_INSPECT(hex(key));
        CC_INSPECT(hex(start));

        CbcBlockCipher cipher{AesBlockCipher{key}, start};
        BlockCipherSource source{MemoryStream{cipherText}, cipher};

        String message = source.readAll();
        ModuloPadding::removePadding(&message);
        CC_INSPECT(message);
    }
    #endif

    //// Yeelight Color Bulb 04:91 07/2018 (firmware 76)
    {
        YeelightSecretMessage message {{
            0x21, 0x31, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x31, 0x63, 0x7e, 0x00, 0x00, 0x00, 0x40,
            0xb7, 0x64, 0x87, 0xc1, 0xaf, 0x5e, 0xb1, 0xd7, 0xa3, 0xe9, 0x2d, 0x2b, 0x85, 0xac, 0x51, 0xab,
            0x42, 0x9b, 0x76, 0x2d, 0x3b, 0x9a, 0xbb, 0x5a, 0xc9, 0xa5, 0x3d, 0x3e, 0xfb, 0x8f, 0x40, 0x44,
            0x22, 0x1e, 0x42, 0x83, 0x49, 0x16, 0x86, 0x03, 0x2b, 0x0e, 0x85, 0x6f, 0xc6, 0xd6, 0x9c, 0x7a,
            0x0b, 0x58, 0x58, 0x8b, 0x01, 0xe2, 0x6e, 0xf1, 0x42, 0xb6, 0xfc, 0x55, 0x5f, 0xcf, 0x00, 0x00,
            0x3b, 0xdc, 0x8e, 0xe8, 0x8f, 0x16, 0x77, 0x84, 0xde, 0x92, 0xf4, 0x62, 0x2e, 0xf4, 0xc9, 0x62,
            0x1a, 0x65, 0x1b, 0x5b, 0xca, 0x4a, 0x8c, 0x8d, 0x4b, 0x56, 0x35, 0x0b, 0x2d, 0x87, 0xd9, 0x2c,
            0x92, 0x21, 0x98, 0x14, 0x8b, 0xf9, 0xee, 0xdf, 0xdd, 0x65, 0xf3, 0x95, 0x20, 0x44, 0xda, 0xba,
            0xe6, 0x1c, 0x18, 0xd4, 0x00, 0xa7, 0xac, 0xc4, 0xa2, 0x67, 0x1d, 0xc9, 0x07, 0xd0, 0x9b, 0x00,
            0x32, 0x46, 0xde, 0x3d, 0x44, 0x25, 0xdb, 0x5e, 0x25, 0x7f, 0x8b, 0x53, 0x8c, 0xb2, 0x71, 0x71,
            0x96, 0xa6, 0xe8, 0xec, 0xc9, 0xab, 0x07, 0xbf, 0xbc, 0xd4, 0xb1, 0xde, 0x16, 0x2a, 0xc7, 0x1c,
            0xd7, 0x91, 0xda, 0x67, 0x42, 0xb3, 0x06, 0xf8, 0x45, 0x73, 0x68, 0x74, 0xcd, 0x85, 0xe7, 0x29,
            0xed, 0x87, 0x35, 0xaf, 0xfc, 0xb0, 0x5e, 0x30, 0x76, 0x83, 0x80, 0x52, 0x18, 0xd4, 0x20, 0xe4,
            0xa4, 0x9b, 0x89, 0x5c, 0x2d, 0xc7, 0xeb, 0x60, 0x57, 0xe4, 0xf6, 0x1d, 0xcf, 0x8c, 0x1c, 0x8b,
            0x78, 0xee, 0x7a, 0x23, 0x4a, 0x5d, 0xff, 0xca, 0x52, 0xc4, 0xfe, 0xae, 0xb4, 0xd8, 0x62, 0x26,
            0x44, 0x15, 0xdb, 0xea, 0xdd, 0xe1, 0x03, 0x0a, 0xdd, 0x97, 0x8e, 0xa5, 0xe2, 0x78, 0xe3, 0xc1
        }};

        Bytes token {
           0x9e, 0x16, 0x57, 0x0e, 0xec, 0x34, 0x58, 0x5b, 0x1d, 0x66, 0x23, 0xb8, 0x22, 0x26, 0xa5, 0x6d
        };

        CC_INSPECT(hex(message.magic()));
        CC_INSPECT(message.length());
        CC_INSPECT(hex(message.deviceId()));
        CC_INSPECT(message.uptime());
        CC_INSPECT(hex(message.checksum()));
        CC_INSPECT(hex(message.computeChecksum(token)));
        CC_INSPECT(message.decipherPayload(token));
    }

    #if 0
    {
        Bytes packet {
            0x21, 0x31, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xf5, 0x10, 0x0c, 0x00, 0x00, 0x00, 0x55,
            0x53, 0x00, 0x87, 0x61, 0x55, 0xf0, 0x8d, 0x0a, 0x9f, 0x1e, 0x10, 0xa0, 0xe1, 0xa2, 0x1e, 0x89,
            0x6d, 0x24, 0x32, 0x5a, 0x39, 0x77, 0xbb, 0xdf, 0x43, 0x90, 0x95, 0xb1, 0x22, 0xa2, 0xe0, 0x69,
            0x1f, 0x24, 0x7e, 0x0c, 0x00, 0x91, 0x9c, 0xb1, 0x6b, 0x27, 0x68, 0x6a, 0x0a, 0x70, 0xb6, 0xd7,
            0x6c, 0xce, 0x0e, 0x11, 0x49, 0x93, 0x10, 0xa7, 0xb7, 0xd8, 0x35, 0x20, 0x76, 0x8b, 0x86, 0x52,
            0xe3, 0x48, 0xce, 0xee, 0xfa, 0x43, 0xcd, 0x41, 0xbe, 0x50, 0x5a, 0x28, 0x07, 0x2d, 0xa4, 0x6c,
            0x96, 0xf4, 0x60, 0xf8, 0xcf, 0xe6, 0x79, 0x42, 0xe2, 0x8a, 0x4a, 0xba, 0x52, 0x8f, 0x3a, 0x03,
            0x4c, 0xcc, 0xaa, 0x07, 0xf5, 0x3c, 0x97, 0x9b, 0x53, 0x99, 0x0e, 0xbc, 0x52, 0xe3, 0xea, 0x6f,
            0x07, 0x81, 0xc8, 0xe2, 0x9b, 0xdd, 0xf9, 0x70, 0xdf, 0x39, 0x51, 0x90, 0xaf, 0xa6, 0x4a, 0x01,
            0x2e, 0xe2, 0xd8, 0x9e, 0x58, 0xf2, 0xf7, 0xbe, 0x02, 0xb1, 0xe7, 0x5a, 0x1e, 0xf7, 0x6b, 0x4b,
            0x08, 0xc6, 0x72, 0x83, 0xf1, 0x91, 0xa4, 0xcd, 0x6d, 0xc0, 0xb2, 0x0c, 0xa2, 0x45, 0x55, 0xce,
            0xdf, 0xc0, 0xbf, 0x5b, 0xa3, 0x10, 0xab, 0xa1, 0xf3, 0x45, 0x6a, 0x1c, 0x65, 0x1e, 0xb7, 0xd8,
            0xe8, 0x17, 0x47, 0xdc, 0x4c, 0x8b, 0xda, 0x06, 0xbe, 0xcb, 0xb4, 0x15, 0xb3, 0x1a, 0x82, 0x12,
            0xcf, 0xbf, 0xee, 0x36, 0xbd, 0xc7, 0xbe, 0x91, 0xcd, 0xb0, 0x80, 0x14, 0x45, 0xba, 0x58, 0x8b,
            0xb5, 0xbf, 0x3b, 0x59, 0x61, 0x68, 0x99, 0xe1, 0xf2, 0xd4, 0x21, 0xf0, 0x90, 0xba, 0x26, 0x2a,
            0xb3, 0x03, 0x03, 0x92, 0x71, 0x82, 0xc9, 0x68, 0xa7, 0xec, 0x97, 0xfd, 0x22, 0x24, 0xe6, 0xc2
        };

        Bytes expectedHash {
            0x24, 0xbf, 0x78, 0x5c, 0xac, 0xaf, 0x64, 0x61, 0xe8, 0xf2, 0x3a, 0x05, 0x54, 0xbd, 0xc1, 0x3a
        };

        CC_INSPECT(hex(md5(packet)));
        CC_INSPECT(hex(expectedHash));
    }
    {
        CC_INSPECT(hex(YeelightSecretHello::produce()));
    }

    {
        YeelightSecretStatus status {{
            0x21, 0x31, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
            0x13, 0xf5, 0x10, 0x0c, 0x00, 0x00, 0x00, 0x55,
            0x53, 0x00, 0x87, 0x61, 0x55, 0xf0, 0x8d, 0x0a,
            0x9f, 0x1e, 0x10, 0xa0, 0xe1, 0xa2, 0x1e, 0x89
        }};

        CC_INSPECT(hex(status.deviceId()));
        CC_INSPECT(hex(status.token()));

        YeelightSecretSetup setup {
            status,
            status.token(),
            "cybslan",
            "franziskaner7"
        };

        HexDump{}.write(setup.message());

        YeelightSecretMessage setup2 { setup.message() };
        CC_INSPECT(hex(setup2.magic()));
        CC_INSPECT(hex(setup2.deviceId()));
        CC_INSPECT(hex(setup2.length()));
        CC_INSPECT(hex(setup2.uptime()));
        CC_INSPECT(hex(setup2.checksum()));
        CC_INSPECT(hex(setup2.computeChecksum(status.token())));

    }

    {
        YeelightSecretMessage message {{
            0x21, 0x31, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xf5, 0x10, 0x0c, 0x00, 0x00, 0x00, 0x55,
            0x24, 0xbf, 0x78, 0x5c, 0xac, 0xaf, 0x64, 0x61, 0xe8, 0xf2, 0x3a, 0x05, 0x54, 0xbd, 0xc1, 0x3a,
            0x6d, 0x24, 0x32, 0x5a, 0x39, 0x77, 0xbb, 0xdf, 0x43, 0x90, 0x95, 0xb1, 0x22, 0xa2, 0xe0, 0x69,
            0x1f, 0x24, 0x7e, 0x0c, 0x00, 0x91, 0x9c, 0xb1, 0x6b, 0x27, 0x68, 0x6a, 0x0a, 0x70, 0xb6, 0xd7,
            0x6c, 0xce, 0x0e, 0x11, 0x49, 0x93, 0x10, 0xa7, 0xb7, 0xd8, 0x35, 0x20, 0x76, 0x8b, 0x86, 0x52,
            0xe3, 0x48, 0xce, 0xee, 0xfa, 0x43, 0xcd, 0x41, 0xbe, 0x50, 0x5a, 0x28, 0x07, 0x2d, 0xa4, 0x6c,
            0x96, 0xf4, 0x60, 0xf8, 0xcf, 0xe6, 0x79, 0x42, 0xe2, 0x8a, 0x4a, 0xba, 0x52, 0x8f, 0x3a, 0x03,
            0x4c, 0xcc, 0xaa, 0x07, 0xf5, 0x3c, 0x97, 0x9b, 0x53, 0x99, 0x0e, 0xbc, 0x52, 0xe3, 0xea, 0x6f,
            0x07, 0x81, 0xc8, 0xe2, 0x9b, 0xdd, 0xf9, 0x70, 0xdf, 0x39, 0x51, 0x90, 0xaf, 0xa6, 0x4a, 0x01,
            0x2e, 0xe2, 0xd8, 0x9e, 0x58, 0xf2, 0xf7, 0xbe, 0x02, 0xb1, 0xe7, 0x5a, 0x1e, 0xf7, 0x6b, 0x4b,
            0x08, 0xc6, 0x72, 0x83, 0xf1, 0x91, 0xa4, 0xcd, 0x6d, 0xc0, 0xb2, 0x0c, 0xa2, 0x45, 0x55, 0xce,
            0xdf, 0xc0, 0xbf, 0x5b, 0xa3, 0x10, 0xab, 0xa1, 0xf3, 0x45, 0x6a, 0x1c, 0x65, 0x1e, 0xb7, 0xd8,
            0xe8, 0x17, 0x47, 0xdc, 0x4c, 0x8b, 0xda, 0x06, 0xbe, 0xcb, 0xb4, 0x15, 0xb3, 0x1a, 0x82, 0x12,
            0xcf, 0xbf, 0xee, 0x36, 0xbd, 0xc7, 0xbe, 0x91, 0xcd, 0xb0, 0x80, 0x14, 0x45, 0xba, 0x58, 0x8b,
            0xb5, 0xbf, 0x3b, 0x59, 0x61, 0x68, 0x99, 0xe1, 0xf2, 0xd4, 0x21, 0xf0, 0x90, 0xba, 0x26, 0x2a,
            0xb3, 0x03, 0x03, 0x92, 0x71, 0x82, 0xc9, 0x68, 0xa7, 0xec, 0x97, 0xfd, 0x22, 0x24, 0xe6, 0xc2
        }};

        Bytes token {
            0x53, 0x00, 0x87, 0x61, 0x55, 0xf0, 0x8d, 0x0a, 0x9f, 0x1e, 0x10, 0xa0, 0xe1, 0xa2, 0x1e, 0x89
        };

        CC_INSPECT(hex(message.magic()));
        CC_INSPECT(message.length());
        CC_INSPECT(hex(message.deviceId()));
        CC_INSPECT(message.uptime());
        CC_INSPECT(hex(message.checksum()));
        CC_INSPECT(hex(message.computeChecksum(token)));
        CC_INSPECT(message.decipherPayload(token));
    }

    {
        Bytes message {
            0x21, 0x31, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x13, 0xf5, 0x10, 0x0c, 0x00, 0x00, 0x00, 0x55,
            0x4e, 0x15, 0xf6, 0x56, 0x07, 0x82, 0x4d, 0x22, 0x22, 0x3c, 0x54, 0x30, 0xa2, 0xb6, 0x98, 0x2d,
            0x6d, 0x24, 0x32, 0x5a, 0x39, 0x77, 0xbb, 0xdf, 0x43, 0x90, 0x95, 0xb1, 0x22, 0xa2, 0xe0, 0x69,
            0x70, 0x44, 0x09, 0x83, 0x6a, 0x14, 0xaa, 0x04, 0x14, 0xd8, 0xc0, 0x8b, 0x24, 0x16, 0x41, 0x7d,
            0x5d, 0xf9, 0xa3, 0x78, 0x1d, 0x20, 0xbe, 0x2f, 0x65, 0x23, 0x59, 0x07, 0xe0, 0xe6, 0x81, 0xd3
        };

        Bytes token {
            0x53, 0x00, 0x87, 0x61, 0x55, 0xf0, 0x8d, 0x0a, 0x9f, 0x1e, 0x10, 0xa0, 0xe1, 0xa2, 0x1e, 0x89
        };

        CC_INSPECT(YeelightSecretMessage{message}.decipherPayload(token));

        YeelightResult result = YeelightSecretResult{message}.decipherResult(token);
        CC_INSPECT(result.isOk());
        CC_INSPECT(result.commandId());
    }
    #endif

    return 0;
}