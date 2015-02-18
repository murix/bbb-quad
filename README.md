Beagleboard Black Quad Copter
=============================

Development a hobby quadcopter using beaglebone black


Setup PC as internet USB router
===============================

sysctl -w net.ipv4.ip_forward=1

iptables -t nat -A POSTROUTING -j MASQUERADE

ifconfig usb0 192.168.7.1

Connect BBB to internet via USB
===============================

ifconfig usb0 192.168.7.2

route add default gw 192.168.7.1

echo nameserver 8.8.8.8 > /etc/resolv.conf

