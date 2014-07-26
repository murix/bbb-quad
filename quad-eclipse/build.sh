#!/bin/bash

echo "build devicetree overlay"
dtc -O dtb -o bone_pru0_out-00A0.dtbo -b 0 -@ bone_pru0_out-00A0.dts

echo "install devicetree overlay"
cp -av *.dtbo /lib/firmware

echo "enable devicetree overlay"
#echo bone_pru0_out > /sys/devices/bone_capemgr.*/slots

echo "build pru firmware"
pasm -b pwm.p

echo "build test program"
g++ -g0 -O3 -Wall -std=c++0x main.cpp pru.cpp pruPWM.cpp -lprussdrv -lpthread -o pwm-test

g++ -g0 -O3 -Wall -std=c++0x -lm -D_BSD_SOURCE gy86.cpp -o gy86

g++ -g0 -O3 -Wall -std=c++0x baro.cpp -o baro




