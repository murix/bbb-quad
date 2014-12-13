#!/usr/bin/python

import os
import time
import threading
from gps import *

os.system("echo BB-UART4  > /sys/devices/bone_capemgr.9/slots")
time.sleep(2)
os.system("gpsdctl add /dev/ttyO4")
 
class GpsPoller(threading.Thread):
   def __init__(self):
       threading.Thread.__init__(self)
       self.session = gps(mode=WATCH_ENABLE)
   def get_s(self):
       return self.session
   def run(self):
       while True:
          try:
            self.session.next()
          except:
            pass
 
if __name__ == '__main__':
   gpsp = GpsPoller()
   gpsp.start()
   while 1:
       time.sleep(1)
       print gpsp.get_s()


