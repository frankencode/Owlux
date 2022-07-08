Owlux: Yeelight control app for Raspberry Pi 3/4
================================================

About
-----

Owlux is a smart light control app. It allows to control the WiFi based Yeelight smart LED
bulbs and ceiling lights.

Supported features:
 * Automatic detection of all lights on your network
 * Keeping track of lights by processing hourly announcements (refresh messages)
 * Switch on/off individual lights
 * Change of temperature, brightness (and color)
 * Sleep timers (works even after closing Owlux)
 * Renaming of lights
 * ... more to come !-)

*For firmware updates and commissioning of lights to your WiFi hotspot you still need to use the official Yeelight app.*

Screenshots
-----------

Running on Raspberry Pi OS 11:

![Owlux on Bullseye 1](screenshots/2022-07-08-220154_360x687_scrot.png)
![Owlux on Bullseye 2](screenshots/2022-07-08-220201_360x687_scrot.png)

Building on your Raspberry Pi
-----------------------------

In the scripts/ subfolder you will find a script to build and install Owlux
from sources. Owlux requires at least Raspberry Pi OS 11 (Debian Bullseye).
Other dependencies will be installed automatically by the installation scripts:

```
wget https://raw.githubusercontent.com/frankencode/Owlux/master/scripts/install_pi_11.sh
./install_pi_11.sh
```

The build process takes about 20min on a Raspberry Pi 400.

**Please make sure that you have proper cooling if you are using a different Raspberry Pi!**

Disclaimer
----------

Yeelight is a trademark Qingdao Yeelink Information Technology Co., Ltd.
The authors of Owlux are not affiliated with Yeelink.
