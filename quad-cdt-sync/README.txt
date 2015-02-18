

#
apt-get install linux-firmware-image-3.8.13-xenomai-r70 linux-headers-3.8.13-xenomai-r70 linux-image-3.8.13-xenomai-r70
#
apt-get install xenomai-kernel-source xenomai-runtime xenomai-runtime-dbg xenomai-doc libxenomai-dev libxenomai1 libxenomai1-dbg

#
apt-get install network-manager ipython

#
apt-get install libjsoncpp-dev libgps-dev gpsd libi2c-dev  strace mc

#
apt-get install bluez  bluez-hcidump checkinstall libusb-dev libbluetooth-dev joystick

#
git clone https://github.com/murix/qtsixa.git
cd qtsixa/sixad
make
checkinstall
systemctl enable sixad
systemctl start sixad


---> /etc/rc.local

#gera panic se kernel travar por 10 segundos
sysctl -w kernel.hung_task_timeout_secs=10
sysctl -w kernel.hung_task_panic=1

#gera panic se kernel pegar simbolo doido
sysctl -w kernel.panic_on_oops=1

#gera panic se kernel sofrer de lockup
sysctl -w kernel.softlockup_panic=1

#gera panic se acabar a RAM
sysctl -w vm.panic_on_oom=1

#reboot automatico em 2 segundos apos um kernel panic
sysctl -w kernel.panic=2

---> /etc/fstab

# 
tmpfs /var/log  tmpfs defaults 0 0 

--->











