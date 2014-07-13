#!/usr/bin/python

import Adafruit_BBIO.PWM as PWM
import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.UART as UART
import Adafruit_BBIO.SPI as SPI


pwm_arm=45

PWM.start("P8_13",pwm_arm,490)
PWM.start("P9_14",pwm_arm,490)
PWM.start("P9_21",pwm_arm,490)
PWM.start("P9_42",pwm_arm,490)

#PWM.set_duty_cycle()

