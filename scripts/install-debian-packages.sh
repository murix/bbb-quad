#!/bin/bash

# get new list of packages
apt-get -y update

# upgrade remain existing packages
apt-get -y upgrade

# install new
INSTALL=""
INSTALL="$INSTALL git-core cmake libjsoncpp-dev htop mc minicom aptitude sshfs nmap ipython"
INSTALL="$INSTALL "
INSTALL="$INSTALL "
apt-get -y install $INSTALL

# clean download cache
apt-get clean

# install python modules
easy_install -U distribute
pip install Adafruit_BBIO

