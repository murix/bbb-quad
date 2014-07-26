#!/bin/bash

mkdir -pv bin

#########################################
echo "build devicetree overlay"

dtc -O dtb -o bin/bone_pru0_out-00A0.dtbo -b 0 -@ bone_pru0_out-00A0.dts
cp -av bin/*.dtbo /lib/firmware

##########################################
echo "build pru firmware"
pasm -b pasm-pwm.p bin/pasm-pwm

###########################################
echo "build test program"
g++ -g0 -O3 -Wall -std=c++0x -lprussdrv -lpthread pru-motor-test.cpp pru.cpp pruPWM.cpp -o bin/pru-motor-test
g++ -g0 -O3 -Wall -std=c++0x -lm -D_BSD_SOURCE    gy86.cpp -o bin/gy86
g++ -g0 -O3 -Wall -std=c++0x                      ms5611-test.cpp -o bin/ms5611-test




