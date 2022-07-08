#!/bin/bash

if [ $(head /etc/debian_version --bytes=3) != "11." ]; then
    echo "Warning! This build script is meant to run on Debian 11 (bullseye)."
    echo "Press ENTER to continue anyways..."
    read
fi

set -ex

#
# Download source and bootsrap the build system
#
mkdir -p /tmp/src
pushd /tmp/src
git clone --recurse-submodules https://github.com/frankencode/Owlux.git
mkdir -p /tmp/build/CoreComponents_bootstrap
pushd /tmp/build/CoreComponents_bootstrap
time /tmp/src/Owlux/CoreComponents/bootstrap
CCBUILD=$PWD/ccbuild

#
# Install dependencies
#
sudo apt install --yes libreadline-dev libcairo2-dev libwebp-dev libsdl2-dev fonts-noto-core fonts-noto-mono

#
# Build and install Owlux
#
mkdir -p /tmp/build/Owlux_release
pushd /tmp/build/Owlux_release
$CCBUILD -configure -release /tmp/src/Owlux/
time $CCBUILD -release /tmp/src/Owlux

sudo $CCBUILD -install -release /tmp/src/Owlux
