
# network ip
ifconfig usb0 192.168.7.2
# network gateway
route add default gw 192.168.7.1
# network DNS
echo nameserver 8.8.8.8 > /etc/resolv.conf

# other packages
apt-get -y install udhcpd cape-firmware mc less dbus ipython sshfs firmware-linux am335x-pru-package firmware-am335x-pm

# adafruit python  BBIO
apt-get install -y build-essential python-dev python-setuptools python-pip python-smbus
easy_install -U distribute
pip install Adafruit_BBIO

#  other libraries
apt-get -y install libgps-dev libmodbus-dev libnl-dev libpthread-stubs0-dev libsnmp-dev libncurses5-dev libmodbus-dev

