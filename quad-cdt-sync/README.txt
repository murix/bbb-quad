

#
apt-get install linux-firmware-image-3.8.13-xenomai-r70 linux-headers-3.8.13-xenomai-r70 linux-image-3.8.13-xenomai-r70
#
apt-get install xenomai-kernel-source xenomai-runtime xenomai-runtime-dbg xenomai-doc libxenomai-dev libxenomai1 libxenomai1-dbg

#
apt-get install network-manager

apt-get install libjsoncpp-dev libgps-dev gpsd libi2c-dev  strace mc


apt-get install bluez  bluez-hcidump checkinstall libusb-dev libbluetooth-dev joystick


git clone https://github.com/murix/qtsixa.git
cd qtsixa/sixad
make
checkinstall
systemctl enable sixad
systemctl start sixad

[  180.504540] "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
[  180.512740] Kernel panic - not syncing: hung_task: blocked tasks
[  180.519031] [<c0010bc9>] (unwind_backtrace+0x1/0x90) from [<c051397f>] (panic+0x8b/0x188)
[  180.527568] [<c051397f>] (panic+0x8b/0x188) from [<c006c095>] (watchdog+0x1c9/0x1e0)
[  180.535649] [<c006c095>] (watchdog+0x1c9/0x1e0) from [<c0045773>] (kthread+0x83/0x84)
[  180.543817] [<c0045773>] (kthread+0x83/0x84) from [<c000c9f7>] (ret_from_fork+0x13/0x3c)
[  180.552239] drm_kms_helper: panic occurred, switching back to text console
[  221.129231] CAUTION: musb: Babble Interrupt Occurred
[  221.181188]  gadget: high-speed config #1: Multifunction with RNDIS
[  227.097846] musb_g_ep0_irq 710: SetupEnd came in a wrong ep0stage wait
[  241.766252] musb_g_ep0_irq 710: SetupEnd came in a wrong ep0stage wait
[  251.777182] musb_g_ep0_irq 710: SetupEnd came in a wrong ep0stage wait
[  261.784248] musb_g_ep0_irq 710: SetupEnd came in a wrong ep0stage wait
[  267.116971] CAUTION: musb: Babble Interrupt Occurred
[  267.166931]  gadget: high-speed config #1: Multifunction with RNDIS

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











