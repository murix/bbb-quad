#!/usr/bin/python
 
import Adafruit_BBIO.GPIO as G
 
G.setup("P8_7",G.OUT,G.PUD_OFF)
G.setup("P8_8",G.OUT,G.PUD_OFF)
 
while True:
 G.output("P8_7",G.HIGH)
 G.output("P8_7",G.LOW)
 G.output("P8_8",G.HIGH)
 G.output("P8_8",G.LOW)
 
