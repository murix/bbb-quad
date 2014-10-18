#!/bin/bash

PURGE=""
INSTALL=""

PURGE="$PURGE nodejs apache2.2-common gnome-icon-theme libx11-doc libopencv-dev"
PURGE="$PURGE libgl1-mesa-dri locales vim vim-runtime freepats libicu48 libflite1"
PURGE="$PURGE libwebkitgtk-1.0-0 libavcodec53 gstreamer0.10-ffmpeg gstreamer0.10-plugins-bad"
PURGE="$PURGE gstreamer0.10-plugins-base gstreamer0.10-plugins-good libopencore-amrnb0 libopencore-amrwb0"
PURGE="$PURGE libavcodec-extra-53"
PURGE="$PURGE python-wicd wicd-cli wicd-curses wicd-daemon wicd-gtk"

INSTALL="$INSTALL  udhcpd dbus python-dbus network-manager network-manager-gnome"
INSTALL="$INSTALL  mc less ipython sshfs aptitude python-matplotlib minicom"
#GPS
INSTALL="$INSTALL  gpsd gpsd-clients python-gps"
#X11
INSTALL="$INSTALL  xterm icewm xrdp tightvncserver lxde-core"
#firmwares
INSTALL="$INSTALL  firmware-linux "
#BBB repo
INSTALL="$INSTALL  am335x-pru-package cape-firmware device-tree-compiler firmware-am335x-pm"
#adafruit BBIO deps
INSTALL="$INSTALL  build-essential python-dev python-setuptools python-pip python-smbus"
#
INSTALL="$INSTALL  libgps-dev libmodbus-dev libnl-dev libpthread-stubs0-dev libsnmp-dev libncurses5-dev libmodbus-dev"
#omap-image-builder deps
INSTALL="$INSTALL  dosfstools git-core kpartx u-boot-tools wget parted pv"


# get new list of packages
apt-get update

# purge
#apt-get -y purge $PURGE

# remove any trash
#apt-get -y autoremove

# purge remain trash
#apt-get -y purge `dpkg --get-selections | grep deinstall | awk '{print $1}'`

# upgrade remain existing packages
apt-get -y upgrade

# install new
apt-get -y install $INSTALL

# clean download cache
apt-get clean

# install python modules
easy_install -U distribute
pip install Adafruit_BBIO






