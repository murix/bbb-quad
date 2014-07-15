#!/usr/bin/python

from time import sleep
import thread
import Adafruit_BBIO.GPIO as G
import curses

ss=curses.initscr()
curses.noecho()

G.cleanup()


try:
##############################
 for p in [ "P9_", "P8_" ]:
  for i in range(1,47):
   pin=p+str(i)
   ss.addstr(1,1,str(pin))
   G.setup(pin,G.IN,G.PUD_OFF)
###############################
except:
 pass

curses.endwin()
print "input test end"




