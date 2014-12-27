#!/bin/bash

echo "---------------- device tree ------------------"
dtc -O dtb -o bone_pru0_out-00A0.dtbo -b 0 -@ bone_pru0_out-00A0.dts
cp -av bone_pru0_out-00A0.dtbo /lib/firmware
pasm -b pru-pwm-fw.asm pru-pwm-fw

echo "---------------- cmake ------------------------"
rm CMakeCache.txt
cmake .

echo "---------------- make ------------------------"
make clean
make




