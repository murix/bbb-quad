#!/usr/bin/python

from time import sleep
import thread
import Adafruit_BBIO.GPIO as G
import curses

ss=curses.initscr()

curses.noecho()

G.cleanup()

y=0
x=0

try:
 ss.addstr(1,1,"dasdasdasda")
 for p in [ "P9_", "P8_" ]:
  if p == "P9_": y=10
  if p == "P8_": y=20
  for i in range(1,47,2):
   x+=1

   pin1=p+str(i)
   pin2=p+str(i+1)

   G.setup(pin1,G.IN,G.PUD_OFF)
   G.setup(pin2,G.IN,G.PUD_OFF)

   ss.addstr(x,y,str(pin1))
except:
 pass

curses.endwin()




