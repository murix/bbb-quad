
ifconfig usb0 192.168.7.2
route add default gw 192.168.7.1
echo nameserver 8.8.8.8 > /etc/resolv.conf
apt-get -y install udhcpd cape-firmware mc less dbus ipython
apt-get install -y build-essential python-dev python-setuptools python-pip python-smbus
easy_install -U distribute
pip install Adafruit_BBIO


