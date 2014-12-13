
# network ip
ifconfig usb0 192.168.5.2
# network gateway
route del default
route add default gw 192.168.5.1
# network DNS
echo nameserver 8.8.8.8 > /etc/resolv.conf

