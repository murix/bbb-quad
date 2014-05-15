#!/usr/bin/python

import Adafruit_BBIO.PWM as PWM
import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.UART as UART
import Adafruit_BBIO.SPI as SPI
import curses
import time
stdscr = curses.initscr()
curses.noecho()

PWM.cleanup()
GPIO.cleanup()

# motor pin map
pin_fl="P9_31"
pin_rl="P9_16"
pin_rr="P8_19"
pin_fr="P9_28"

# stay off
GPIO.setup(pin_fl,GPIO.OUT,0)
GPIO.setup(pin_fr,GPIO.OUT,0)
GPIO.setup(pin_rl,GPIO.OUT,0)
GPIO.setup(pin_rr,GPIO.OUT,0)
time.sleep(1)



# pwm to arm motor
arm_fl=40
arm_fr=40
arm_rl=40
arm_rr=40

# setup PWM to arm motors
PWM.start(pin_fl,arm_fl,490)
PWM.start(pin_fr,arm_fr,490)
PWM.start(pin_rl,arm_rl,490)
PWM.start(pin_rr,arm_rr,490)

# init PWM control vars
pwm_fl=arm_fl
pwm_fr=arm_fr
pwm_rl=arm_rl
pwm_rr=arm_rr

key = 'q'

def print_pwm():
  #stdscr.clrscr()
  stdscr.addstr(0,0,"key="+key)
  stdscr.addstr(1,5,"front left (7,8)="+str(pwm_fl))
  stdscr.addstr(1,30,"front right (9,+)="+str(pwm_fr))
  stdscr.addstr(2,5,"rear left (4,5)="+str(pwm_rl))
  stdscr.addstr(2,30,"rear right (6,.)="+str(pwm_rr))
  #stdscr.refresh()

print_pwm()

try:
 while True:
  #
  key = chr(stdscr.getch())

  # CONTROL
  if key == '7': pwm_fl-=1
  if key == '8': pwm_fl+=1
  if key == '9': pwm_fr-=1
  if key == '+': pwm_fr+=1
  if key == '4': pwm_rl-=1
  if key == '5': pwm_rl+=1
  if key == '6': pwm_rr-=1
  if key == '.': pwm_rr+=1

  # QUIT
  if key == 'q':
   break

  # START ALL
  if key == 's':
   pwm_fl=47
   pwm_fr=47
   pwm_rl=47
   pwm_rr=47

  # ARM
  if key == 'a':
   pwm_fl=arm_fl
   pwm_fr=arm_fr
   pwm_rl=arm_rl
   pwm_rr=arm_rr


  print_pwm()

  PWM.set_duty_cycle(pin_fl,pwm_fl)
  PWM.set_duty_cycle(pin_fr,pwm_fr)
  PWM.set_duty_cycle(pin_rl,pwm_rl)
  PWM.set_duty_cycle(pin_rr,pwm_rr)

except:
 pass

curses.endwin()
PWM.cleanup()
