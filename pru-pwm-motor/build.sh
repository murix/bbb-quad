#!/bin/bash

echo "build devicetree overlay"
dtc -O dtb -o bone_pru0_out-00A0.dtbo -b 0 -@ bone_pru0_out-00A0.dts

echo "install devicetree overlay"
cp -av *.dtbo /lib/firmware

echo "enable devicetree overlay"
echo bone_pru0_out > /sys/devices/bone_capemgr.*/slots

echo "build pru firmware"
pasm -b pwm.p

echo "build test program"
g++ -g0 -O3 -Wall main.cpp pru.cpp pruPWM.cpp -lprussdrv -lpthread -o pwm-test





