#!/usr/bin/python

from time import sleep
import thread
import Adafruit_BBIO.GPIO as G

def function_pin_change(pin,time):
 s=True
 while True:
  s= not s
  G.output(pin,s)
  sleep(time)
  

for p in [ "P9_", "P8_" ]:
 for i in range(1,47):
  pin=p+str(i)
  print pin,
  #G.setup(pin,G.OUT,G.PUD_OFF,0)
  G.setup(pin,G.OUT,G.PUD_DOWN,0)
  #G.setup(pin,G.OUT,G.PUD_UP,0)
  #G.setup(pin,G.OUT,G.PUD_OFF,1)
  #G.setup(pin,G.OUT,G.PUD_DOWN,1)
  #G.setup(pin,G.OUT,G.PUD_UP,1)
  func_args = ( pin, 0.5 )
  thread.start_new_thread(function_pin_change, func_args )
  

print ""
print "all pins change state every half second foverer"
while 1:
 pass





