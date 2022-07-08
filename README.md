Owlux: Yeelight smart LED control app for Raspberry Pi 3/4
==========================================================

About
-----

Owlux is a smart light control app. It allows to control the WiFi based Yeelight smart LED
bulbs and ceiling lights.

Supported features:
 * automatic detection of all lights on your network
 * keeping track of lights by processing hourly announcements (refresh messages)
 * switch on/off individual lights
 * change of temperature, brightness (and color)
 * sleep timers (works even after closing Owlux)
 * renaming of lights

Devices known to work:
 * Yeelight 1S (aka color4)
 * Yeelight 9W (aka color)
 * Yeelight Galaxy Ceiling Light 450

*(For firmware updates and commissioning of lights to your WiFi hotspot you still need to use the official Yeelight app.)*

Screenshots
-----------

Running on Raspberry Pi OS 11:

![Owlux on Bullseye 1](screenshots/2022-07-08-220154_360x687_scrot.png)
![Owlux on Bullseye 2](screenshots/2022-07-08-220201_360x687_scrot.png)

Building on your Raspberry Pi
-----------------------------

In the scripts/ subfolder you will find a script to build and install Owlux
from sources. Owlux requires at least Raspberry Pi OS 11 (Debian Bullseye).
Other dependencies will be installed automatically by the installation script.

Open the terminal and issue the following command to build and install Owlux:

```sh
wget https://raw.githubusercontent.com/frankencode/Owlux/master/scripts/install_pi_11.sh && time bash ./install_pi_11.sh
```

The build process takes about 25min on a Raspberry Pi 400. When the build has finished you will find a link on your desktop to start the application.

**Please make sure that you have proper cooling if you are using a different Raspberry Pi!**

Disclaimer
----------

Yeelight is a trademark Qingdao Yeelink Information Technology Co., Ltd.
The authors of Owlux are not affiliated with Yeelink.
