
# get new list of packages
apt-get update

# remove unused packages
apt-get -y purge nodejs
apt-get -y purge freepats vim-runtime locales lxde-core libgl1-mesa-dri libicu48 libopencv-dev
apt-get -y purge desktop-base libruby1.9.1 gnome-icon-theme libavcodec53 
apt-get -y purge  apache2.2-common 
apt-get -y purge libflite1 libavcodec-extra-53  libx11-doc
apt-get -y purge libgtk-3-common

# purge remain trash
apt-get -y purge `dpkg --get-selections | grep deinstall | awk '{print $1}'`

# remove any trash
apt-get -y autoremove

# upgrade existing packages
apt-get -y upgrade

# other packages
apt-get -y install udhcpd cape-firmware dbus python-dbus
#
apt-get -y install mc less ipython sshfs aptitude python-matplotlib
#
apt-get -y install xterm icewm xrdp tightvncserver
# firmware
apt-get -y install firmware-linux firmware-am335x-pm
# PRU
apt-get -y install am335x-pru-package 
# adafruit python  BBIO
apt-get install -y build-essential python-dev python-setuptools python-pip python-smbus
easy_install -U distribute
pip install Adafruit_BBIO
#  other libraries
apt-get -y install libgps-dev libmodbus-dev libnl-dev libpthread-stubs0-dev libsnmp-dev libncurses5-dev libmodbus-dev
# image builder
apt-get -y install dosfstools git-core kpartx u-boot-tools wget parted pv

# clean download cache
apt-get clean



