# other packages
apt-get -y install udhcpd cape-firmware dbus python-dbus
#
apt-get -y install mc less ipython sshfs 
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

