#!/bin/bash


dtc -O dtb -o /lib/firmware/bone_pru0_out-00A0.dtbo -b 0 -@ bone_pru0_out-00A0.dts
pasm -b pru-pwm-fw.asm  /lib/firmware/bone_pru0_out-00A0

