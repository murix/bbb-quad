#!/usr/bin/python

import Adafruit_BBIO.GPIO as G

for p in [ "P9_", "P8_" ]:
 for i in range(1,47):
  pin=p+str(i)
  print pin
  #G.setup(pin,G.OUT,G.PUD_OFF,0)
  G.setup(pin,G.OUT,G.PUD_DOWN,0)
  #G.setup(pin,G.OUT,G.PUD_UP,0)
  #G.setup(pin,G.OUT,G.PUD_OFF,1)
  #G.setup(pin,G.OUT,G.PUD_DOWN,1)
  #G.setup(pin,G.OUT,G.PUD_UP,1)





