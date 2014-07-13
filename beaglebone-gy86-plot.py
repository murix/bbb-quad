#!/usr/bin/python

from Adafruit_I2C import Adafruit_I2C
from time import sleep
import math
import numpy as np
from collections import deque
import sys
import matplotlib.pyplot as plt 
import matplotlib.animation as animation
import os

os.system("echo BB-I2C1  > /sys/devices/bone_capemgr.9/slots")
os.system("echo BB-UART1  > /sys/devices/bone_capemgr.9/slots")

def gy86_read():
 mpu6050 = Adafruit_I2C(0x68,2) # ADC 16bit
 hmc5883 = Adafruit_I2C(0x1e,2) # ADC 12bit
 ms5611  = Adafruit_I2C(0x77,2) # ADC 24bit

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
 temp_mpu6050 = (mpu6050.readS8(0x41)*256+mpu6050.readU8(0x42))/340.0 + 36.53
 #read mpu6050 gyroscope in degree/s
 gx = (mpu6050.readS8(0x43)*256+mpu6050.readU8(0x44))/131.0
 gy = (mpu6050.readS8(0x45)*256+mpu6050.readU8(0x46))/131.0
 gz = (mpu6050.readS8(0x47)*256+mpu6050.readU8(0x48))/131.0

 #####################################################
 # hmc5883 continuous mode
 hmc5883.write8(0x02,0x00)
 #wait hmc5883 to be ready
 while not hmc5883.readU8(0x09)&0x01 == 1: pass
 #read hmc5883 magnetometer in gauss
 magx = (hmc5883.readS8(0x3)*256+hmc5883.readU8(0x4))/1090.0
 magz = (hmc5883.readS8(0x5)*256+hmc5883.readU8(0x6))/1090.0
 magy = (hmc5883.readS8(0x7)*256+hmc5883.readU8(0x8))/1090.0

 #####################################################
 # angles

 z=math.degrees(-math.atan2(magy,magx))
 y=math.degrees(-math.atan2(ax,az));
 x=math.degrees(math.atan2(ay,az));

 #####################################################
 # ms5611 reset
 ms5611.writeList(0x1e,[])
 sleep(3/1000.0)

 # ms5611 prom
 t=ms5611.readList(0xa2,2)
 c1=t[0]*256+t[1]
 t=ms5611.readList(0xa4,2)
 c2=t[0]*256+t[1]
 t=ms5611.readList(0xa6,2)
 c3=t[0]*256+t[1]
 t=ms5611.readList(0xa8,2)
 c4=t[0]*256+t[1]
 t=ms5611.readList(0xaa,2)
 c5=t[0]*256+t[1]
 t=ms5611.readList(0xac,2)
 c6=t[0]*256+t[1]
 #c1=40127
 #c2=36924
 #c3=23317
 #c4=23282
 #c5=33464
 #c6=28312

 #0.065=1ms=0x40
 #0.042=3ms=0x42
 #0.027=4ms=0x44
 #0.018=6ms=0x46
 #0.012=10ms=0x48


 ms5611.writeList(0x48,[])
 sleep(10/1000.0)
 adc=ms5611.readList(0x00,3)
 d1=adc[0]*65536+adc[1]*256+adc[2]
 #d1=9085466

 ms5611.writeList(0x58,[])
 sleep(10/1000.0)
 adc=ms5611.readList(0x00,3)
 d2=adc[0]*65536+adc[1]*256+adc[2]
 #d2=8569150
	
 dt = d2 - c5*(1<<8)
 temp=2000+ dt*c6/(1<<23)
 off= c2*(1<<16) + c4*dt/(1<<7)
 sens=c1*(1<<15) + c3*dt/(1<<8)
 p=(d1*sens/(1<<21) - off)/(1<<15)

 #print c1,c2,c3,c4,c5,c6,d1,d2,dt,temp,off,sens,p
 temp_ms5611=temp/100.0
 press=p/100.0
 p0=1013.25

 alt = 44330.0*(1.0-math.pow((press/p0),1.0/5.255)) 

 out = [ ax,ay,az,temp_mpu6050,gx,gy,gz,x,y,z,magx,magy,magz,temp_ms5611,press,alt*100 ]
 return out

# plot class
class AnalogPlot:
  # constr
  def __init__(self, maxLen):
      self.ax = deque([0.0]*maxLen)
      self.ay = deque([0.0]*maxLen)
      self.az = deque([0.0]*maxLen)
      self.maxLen = maxLen

  # add to buffer
  def addToBuf(self, buf, val):
      if len(buf) < self.maxLen:
          buf.append(val)
      else:
          buf.pop()
          buf.appendleft(val)

  # add data
  def add(self, data):
      assert(len(data) == 3)
      self.addToBuf(self.ax, data[0])
      self.addToBuf(self.ay, data[1])
      self.addToBuf(self.az, data[2])

  # update plot
  def update(self, frameNum, a0, a1,a2):
      try:
          out = gy86_read()
          print out
          [ ax,ay,az,temp_mpu6050,gx,gy,gz,x,y,z,magx,magy,magz,temp_ms5611,press,alt ] = out
          
          #data = [ x,gx, 0 ]
          #data = [ y,gy, 0 ]
          #data = [ z,gz, 0 ]
          data = [ alt , 0,0 ]
          if(len(data) == 3):
              self.add(data)
              a0.set_data(range(self.maxLen), self.ax)
              a1.set_data(range(self.maxLen), self.ay)
              a2.set_data(range(self.maxLen), self.az)
      except KeyboardInterrupt:
          print('exiting')
      
      return a0, 


analogPlot = AnalogPlot(100)
fig = plt.figure()
ax = plt.axes(xlim=(0, 100), ylim=(-300, -200))
a0, = ax.plot([], [])
a1, = ax.plot([], [])
a2, = ax.plot([], [])
anim = animation.FuncAnimation(fig, analogPlot.update, fargs=(a0, a1 ,a2), interval=16)
plt.show()


