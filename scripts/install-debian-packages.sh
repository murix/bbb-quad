#!/bin/bash

#########################################################################################################
#########################################################################################################
#########################################################################################################

PURGE=""
PURGE="$PURGE python-wicd wicd-cli wicd-curses wicd-daemon wicd-gtk"
PURGE="$PURGE network-manager network-manager-gnome"


#########################################################################################################
#########################################################################################################
#########################################################################################################

INSTALL=""

#
INSTALL="$INSTALL cmake libjsoncpp-dev"

INSTALL="$INSTALL  udhcpd dbus python-dbus "
INSTALL="$INSTALL  mc less ipython sshfs aptitude python-matplotlib minicom tcpdump"
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

#########################################################################################################
#########################################################################################################
#########################################################################################################


# get new list of packages
apt-get -y update

# purge
apt-get -y purge $PURGE

# remove any trash
apt-get -y autoremove

# upgrade remain existing packages
apt-get -y upgrade

# install new
apt-get -y install $INSTALL

# clean download cache
apt-get clean

# install python modules
easy_install -U distribute
pip install Adafruit_BBIO

