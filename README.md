bbb-scripts
===========





initial setup in PC
===================



sysctl -w net.ipv4.ip_forward=1
iptables -t nat -A POSTROUTING -j MASQUARE
ifconfig usb0 192.168.7.1
minicom -c on -o -D /dev/ttyACM0

initial setup in beaglebone
===========================
ifconfig usb0 192.168.7.2
route add default gw 192.168.7.1
echo nameserver 8.8.8.8 > /etc/resolv.conf

git clone git@github.com:murix/bbb-scripts.git

git config --global user.name "murix"

git config --global user.email murilopontes@gmail.com

git commit --amend --reset-author

./checkup.sh


