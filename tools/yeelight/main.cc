/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightDiscovery>
#include <owlux/YeelightControl>
#include <owlux/YeelightPower>
#include <owlux/YeelightSetBrightness>
#include <owlux/YeelightSetColor>
#include <owlux/YeelightSetColorTemp>
#include <owlux/YeelightSetHueSat>
#include <owlux/YeelightSetName>
#include <owlux/YeelightSave>
#include <owlux/YeelightStartColorFlow>
#include <owlux/YeelightStopColorFlow>
#include <owlux/YeelightEstablishColorFlow>
#include <owlux/YeelightEstablishAutoDelayOff>
#include <owlux/YeelightStartOffTimer>
#include <owlux/YeelightStopOffTimer>
#include <owlux/setup/YeelightSecretHello>
#include <owlux/setup/YeelightSecretStatus>
#include <owlux/setup/YeelightSecretSetup>
#include <owlux/setup/YeelightSecretResult>
#include <cc/NetworkInterface>
#include <cc/NetworkState>
#include <cc/DatagramSocket>
#include <cc/Arguments>
#include <cc/System>
#include <cc/Date>
#include <cc/Uri>
#include <cc/HexDump>
#include <cc/Date>
#include <cc/stdio>
#include <tuple>
#include <cc/DEBUG>
#include <readline/readline.h>
#include <unistd.h>
#include <termios.h>

namespace cc::owlux {

String getString(const String &prompt)
{
    char *line = readline(prompt);
    String text { line };
    ::free((void *)line);
    return text;
}

String getPassword(const String &prompt)
{
    IoStream::input().echo(false);

    char *line = readline(prompt);
    String text { line };
    ::free((void *)line);

    IoStream::input().echo(true);

    return text;
}

} // namespace cc::owlux

int main(int argc, char *argv[])
{
    using namespace cc;
    using namespace cc::owlux;

    String toolName = String{argv[0]}.baseName();
    int exitStatus = 0;

    Map<String, Variant> options {
        { "scan", false },
        { "refresh", 3 },
        { "monitor", false },
        { "setup", false },
        { "token", "" },
        { "ping", false },
        { "list", false },
        { "listen", false },
        { "delay", 0 },
        { "on", false },
        { "off", false },
        { "bright", 0 },
        { "rgb", 0 },
        { "ct", 0 },
        { "hue", -1 },
        { "sat", -1 },
        { "name", "" },
        { "save", false },
        { "flow", "" },
        { "alarm", false },
        { "stop", false },
        { "sleep", -1 },
        { "sleep-stop", false }
    };

    List<SocketAddress> address;

    try {
        List<String> items = Arguments{argc, argv}.read(&options);

        List<SocketAddress> targets;

        Map<String, SocketAddress> targetByName;
        bool scanFirstTime = true;

        const int port = (options("setup") || options("ping")) ? 54321 : 55443;

        for (const String &item: items) {
            Uri uri{item};
            if (uri.scheme() == "yl" || uri.scheme() == "yeelight" || !uri.isHostNumeric()) {
                if (scanFirstTime) {
                    scanFirstTime = false;
                    long refresh = options("refresh").to<long>();
                    YEELIGHT_EXPECT(refresh > 0);
                    for (YeelightStatus status: YeelightDiscovery{500, 1}) {
                        targetByName.insert(status.name(), status.address());
                        CC_INSPECT(status.name());
                        CC_INSPECT(status.address());
                        if (items.count() == 1 && uri.host() == status.name()) break;
                    }
                }
                SocketAddress address;
                if (targetByName.lookup(uri.host(), &address)) {
                    if (address.port() == 0) address.setPort(port);
                    targets.append(address);
                }
            }
            else {
                targets.append(SocketAddress{Uri{item}, port});
            }
        }

        if (options("monitor")) {
            YEELIGHT_EXPECT(targets.count() == 1);
            YeelightControl control{targets[0]};
            for (YeelightResponse response: control.responseChannel()) {
                YeelightUpdate update{response};
                if (update) fout() << update << nl;
            }
        }
        else if (options("setup") || options("ping"))
        {
            YEELIGHT_EXPECT(targets.count() <= 1);

            SocketAddress peerAddress;
            SocketAddress localAddress;

            if (targets.count() == 1) {
                peerAddress = targets[0];
                localAddress = SocketAddress{ProtocolFamily::Inet4};
            }
            else {
                List<NetworkInterface> interfaces = NetworkInterface::queryAll(ProtocolFamily::Inet4);
                NetworkInterface wifi;
                for (const NetworkInterface &candidate: interfaces) {
                    if (candidate.name().startsWith("wlp") || candidate.name().startsWith("wlan")) {
                        if (candidate.labels().count() > 0) {
                            wifi = candidate;
                            break;
                        }
                    }
                }
                if (!wifi) throw UsageError{"Please connect your WiFi to the bulb hotspot, first."};

                CC_INSPECT(wifi.name());

                peerAddress = NetworkState{wifi.name()}.gateway();
                localAddress = wifi.labels().at(0).address();
                peerAddress.setPort(54321);
            }

            CC_INSPECT(peerAddress);

            DatagramSocket socket{localAddress};
            CC_INSPECT(socket.getLocalAddress());
            // socket.join(SocketAddress{ProtocolFamily::Inet4, "239.255.255.250"});

            String networkName;
            String networkPassword;

            if (!options("ping")) {
                networkName = getString("WiFi name (SSID): ");
                networkPassword = getPassword("WiFi password: ");
            }

            Bytes hello = YeelightSecretHello::produce();
            HexDump{}.write(hello);

            socket.send(YeelightSecretHello::produce(), peerAddress);

            Bytes buffer = Bytes::allocate(1024);

            Bytes message;
            {
                int n = socket.receive(&buffer);
                message = buffer.copy(0, n);
            }

            HexDump{}.write(message);

            YeelightSecretStatus status{message};
            CC_INSPECT(hex(status.deviceId()));

            HexDump{}.write(status.token());

            if (options("ping")) {
                fout() << "Device ID: " << status.deviceId() << nl;
                fout() << "Uptime: " << status.uptime() << "s" << nl;
                if (status.hasToken()) {
                    fout() << "Token: 0x" << hex(status.token()) << nl;
                }
            }
            else if (options("setup")) {
                Bytes token = status.hasToken() ? status.token() : readHex(options("token").to<String>());
                CC_INSPECT(hex(token));
                if (token.count() == 0) {
                    throw UsageError{"Please set the light to factory settings, first. Switch the light 5 times on and off (2s on, 2s off)."};
                }

                YeelightSecretSetup setup{status, token, networkName, networkPassword};

                HexDump{}.write(setup.message());

                socket.send(setup.message(), peerAddress);

                {
                    int n = socket.receive(&buffer);
                    message = buffer.copy(0, n);
                }

                HexDump{}.write(message);

                YeelightResult result = YeelightSecretResult{message}.decipherResult(token);
                CC_INSPECT(result);
            }
        }
        else if (options("list")) {
            YEELIGHT_EXPECT(targets.count() == 0);

            Map<std::tuple<String, SocketAddress>, YeelightStatus> devices;

            for (YeelightStatus status: YeelightDiscovery{500, 1}) {
                devices.insert(std::make_tuple(status.name(), status.address()), status);
            }

            for (auto &pair: devices) {
                YeelightStatus status = pair.value();
                fout()
                    << status.name() << "\t"
                    << status.address().networkAddress() << "\t"
                    << status.id() << "\t"
                    << "0x" << hex(status.model()) << "\t"
                    << status.firmwareVersion() << "\t"
                    << status.colorMode() << "\t"
                    << status.color() << "\t"
                    << nl;
            }
        }
        else if (options("listen")) {
            YEELIGHT_EXPECT(targets.count() == 0);

            for (YeelightStatus status: YeelightDiscovery{}) {
                fout() << "(" << Date{System::now()} << ") " << status << nl;
            }
        }
        else if (options("delay")) {
            YEELIGHT_EXPECT(targets.count() > 0);
            int delay = options("delay").to<long>();
            int bright = options("bright").to<long>();
            if (bright == 0) bright = 100;
            YeelightEstablishAutoDelayOff command{delay, bright};
            CC_INSPECT(command);
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("on") || options("off")) {
            YEELIGHT_EXPECT(targets.count() > 0);
            bool on = options("on").to<bool>();
            YeelightPower command{on};
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("bright")) {
            YEELIGHT_EXPECT(targets.count() > 0);
            int bright = options("bright").to<long>();
            YeelightSetBrightness command{bright};
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("rgb")) {
            YEELIGHT_EXPECT(targets.count() > 0);
            Color color = static_cast<std::uint32_t>(options("rgb").to<long>());
            YeelightSetColor command{color};
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("ct")) {
            YEELIGHT_EXPECT(targets.count() > 0);
            int colorTemp = static_cast<int>(options("ct").to<long>());
            YeelightSetColorTemp command{colorTemp};
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("hue").to<long>() >= 0 || options("sat").to<long>() >= 0) {
            YEELIGHT_EXPECT(targets.count() > 0);
            int hue = options("hue").to<long>();
            int sat = options("sat").to<long>();
            YEELIGHT_EXPECT(0 <= hue && hue <= 359);
            YEELIGHT_EXPECT(0 <= sat && sat <= 100);
            YeelightSetHueSat command{hue, sat};
            CC_INSPECT(command);
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("name").to<String>() != "") {
            YEELIGHT_EXPECT(targets.count() == 1);
            String name = options("name").to<String>();
            YeelightSetName command{name};
            YeelightResult result = YeelightControl{targets[0]}.execute(command);
            fout() << result << nl;
        }
        else if (options("save")) {
            YEELIGHT_EXPECT(targets.count() >= 1);
            YeelightSave command;
            CC_INSPECT(command);
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("flow").to<String>() != "") {
            YEELIGHT_EXPECT(targets.count() >= 1);
            YeelightStartColorFlow command{options("flow").to<String>()};
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("alarm")) {
            YEELIGHT_EXPECT(targets.count() >= 1);
            YeelightEstablishColorFlow command{YeelightColorFlow::alarm()};
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("stop")) {
            YeelightStopColorFlow command;
            YEELIGHT_EXPECT(targets.count() >= 1);
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("sleep").to<long>() >= 0) {
            CC_INSPECT(options("sleep").to<long>());
            int delayInMinutes = options("sleep").to<long>();
            double nowTime = System::now();
            double offTime = nowTime + 60 * delayInMinutes;
            YeelightStartOffTimer command { delayInMinutes };
            fout() << "Going to sleep at " << Date{offTime}.toString() << nl;
            YEELIGHT_EXPECT(targets.count() >= 1);
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else if (options("sleep-stop")) {
            YeelightStopOffTimer command;
            YEELIGHT_EXPECT(targets.count() >= 1);
            for (const SocketAddress &target: targets) {
                YeelightResult result = YeelightControl{target}.execute(command);
                fout() << result << nl;
            }
        }
        else /* if (options("scan") */ {
            long refresh = options("refresh").to<long>();
            YEELIGHT_EXPECT(refresh > 0);
            for (YeelightStatus status: YeelightDiscovery{1000 * refresh}) {
                fout() << "(" << Date{System::now()} << ") " << status << nl;
            }
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [TARGET|INTERFACE]...\n"
            "Discover and control Yeelight devices\n"
            "\n"
            "TARGET is either an IP address or an URL. A special URL scheme \"yl://\" is supported\n"
            "to address Yeelights by name.\n"
            "\n"
            "Options (setup):\n"
            "  -scan           Discover devices on the local network\n"
            "    -refresh=<N>  Refresh interval for scanning in seconds\n"
            "  -monitor        Monitor state changes of TARGET\n"
            "  -setup          Setup WiFi network settings (optionally accepts an INTERFACE)\n"
            "  -ping           Query device ID and uptime (optionally accepts an INTERFACE)\n"
            "\n"
            "Options (operation):\n"
            "  -list           List available devices\n"
            "  -listen         Listen for device anouncements\n"
            "  -on             Switch the light for given TARGET on\n"
            "  -off            Switch the light for given TARGET off\n"
            "  -delay=<N>      Kill the light after the given number of minutes\n"
            "  -bright=<N>     Set brightness to N (1..100)\n"
            "  -hue=<N>        Set the hue for the given TARGET (0..359)\n"
            "    -sat=<N>      Set the saturation of the given TARGET (0..100)\n"
            "  -rgb=<N>        Set color to N (0xRRGGBB)\n"
            "  -ct=<N>         Set color temperatur to N Kelvin (1700..6500)\n"
            "  -name=<S>       Set the device name to S\n"
            "  -save           Save current light settings as default\n"
            "  -flow=<E>       Start color flow expression E\n"
            "  -stop           Stop color flow\n"
            "  -sleep=<N>      Switch off light in <N> minutes\n"
            "  -sleep-stop     Stop sleep timer\n"
            "\n"
        );
    }
    catch (Exception &ex)
    {
        ferr() << ex << nl;
        exitStatus = 4;
    }

    return exitStatus;
}
