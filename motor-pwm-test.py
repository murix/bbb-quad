#!/usr/bin/python

import Adafruit_BBIO.PWM as PWM
import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.UART as UART
import Adafruit_BBIO.SPI as SPI
import curses
stdscr = curses.initscr()
curses.noecho()

pwm_arm=45

pin_fr="P8_13"
pin_rr="P9_14"
pin_rl="P9_21"
pin_fl="P9_42"

PWM.start(pin_fl,pwm_arm,490)
PWM.start(pin_fr,pwm_arm,490)
PWM.start(pin_rl,pwm_arm,490)
PWM.start(pin_rr,pwm_arm,490)

#PWM.set_duty_cycle()

pwm_fl=pwm_arm
pwm_fr=pwm_arm
pwm_rl=pwm_arm
pwm_rr=pwm_arm

while True:
# try:
#  stdscr.clear()
  key = chr(stdscr.getch())
  if key == '7': pwm_fl+=1
  if key == '8': pwm_fl-=1
  if key == '9': pwm_fr+=1
  if key == '+': pwm_fr-=1
  if key == '4': pwm_rl+=1
  if key == '5': pwm_rl-=1
  if key == '6': pwm_rr+=1
  if key == '.': pwm_rr-=1


  stdscr.addstr(0,0,"key="+key)
  stdscr.addstr(1,5,"front left (7,8)="+str(pwm_fl))
  stdscr.addstr(1,30,"front right (9,+)="+str(pwm_fr))
  stdscr.addstr(2,5,"rear left (4,5)="+str(pwm_rl))
  stdscr.addstr(2,30,"rear right (6,.)="+str(pwm_rr))

  PWM.set_duty_cycle(pin_fl,pwm_fl)
  PWM.set_duty_cycle(pin_fr,pwm_fr)
  PWM.set_duty_cycle(pin_rl,pwm_rl)
  PWM.set_duty_cycle(pin_rr,pwm_rr)


  if key=='q':
   break

  stdscr.refresh()
# except:
#  pass

curses.endwin()
PWM.cleanup()
