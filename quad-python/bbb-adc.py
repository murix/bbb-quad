#!/usr/bin/python


from Adafruit_BBIO import ADC 
import time


ADC.setup()

while(True):
 print ("%(adc).3f")% {'adc': (ADC.read_raw("AIN0")/1000.0) * (5.0/0.265) }
 time.sleep(0.1)






