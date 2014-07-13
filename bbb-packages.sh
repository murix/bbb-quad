
# get new list of packages
apt-get update

# remove unwanted
apt-get -y remove nodejs apache2.2-common gnome-icon-theme libx11-doc libopencv-dev libgl1-mesa-dri locales vim vim-runtime freepats libicu48 libflite1 libgtk-3-common iso-codes libwebkitgtk-1.0-0 libavcodec53 gstreamer0.10-ffmpeg gstreamer0.10-plugins-bad gstreamer0.10-plugins-base gstreamer0.10-plugins-good libopencore-amrnb0 libopencore-amrwb0 libavcodec-extra-53

# remove any trash
apt-get -y autoremove

# purge remain trash
apt-get -y purge `dpkg --get-selections | grep deinstall | awk '{print $1}'`

# upgrade existing packages
apt-get -y upgrade

# other packages
apt-get -y install udhcpd cape-firmware dbus python-dbus

#
apt-get -y install mc less ipython sshfs aptitude python-matplotlib minicom 
apt-get -y install gpsd gpsd-clients python-gps

#
apt-get -y install xterm icewm xrdp tightvncserver lxde-core

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



