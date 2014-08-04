#!/usr/bin/python

from Adafruit_I2C import Adafruit_I2C
import time
import math
import sys
import os

os.system("echo BB-I2C1  > /sys/devices/bone_capemgr.9/slots")
time.sleep(2);

def gy86_read():
 mpu6050 = Adafruit_I2C(0x68,2) # ADC 16bit
 #####################################################
 # mpu6050 i2c master enable bit
 mpu6050.write8(0x6A, 0)
 # mpu6050 i2c bypass enable bit
 mpu6050.write8(0x37, 2)
 # mpu6050 turn off sleep mode
 mpu6050.write8(0x6B, 0)

 #read mpu6050 accelerations in m/s^2
 ax = (mpu6050.readS8(0x3b)*256+mpu6050.readU8(0x3c))/16384.0
 ay = (mpu6050.readS8(0x3d)*256+mpu6050.readU8(0x3e))/16384.0
 az = (mpu6050.readS8(0x3f)*256+mpu6050.readU8(0x40))/16384.0
 #read mpu6050 temperature in C
 tc = (mpu6050.readS8(0x41)*256+mpu6050.readU8(0x42))/340.0 + 36.53
 #read mpu6050 gyroscope in degree/s
 gx = (mpu6050.readS8(0x43)*256+mpu6050.readU8(0x44))/131.0
 gy = (mpu6050.readS8(0x45)*256+mpu6050.readU8(0x46))/131.0
 gz = (mpu6050.readS8(0x47)*256+mpu6050.readU8(0x48))/131.0

 print ax,ay,az,tc,gx,gy,gz


while 1:
    gy86_read()


