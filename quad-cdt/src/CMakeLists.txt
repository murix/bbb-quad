
# http://www.cmake.org/cmake/help/cmake_tutorial.html

cmake_minimum_required (VERSION 2.6)

project (bbb-multicopter)

set(CMAKE_CXX_FLAGS "-std=c++0x")

# cc1101 spi tap
#add_executable(test-spi-cc1101-tap cc1101_linux.cpp)

# vbat
#add_executable(test-adc-vbat bbb-adc.cpp bbb-adc-test.cpp)

# barometer
#add_executable(test-i2c-gy86-ms5611  ms5611-test.cpp  bbb_i2c.cpp ms5611.cpp timestamps.cpp)

# accelerometer and gyroscope
#add_executable(test-i2c-gy86-mpu6050 mpu6050-test.cpp bbb_i2c.cpp mpu6050.cpp timestamps.cpp)

# magnetometer
#add_executable(test-i2c-gy86-hmc5883 hmc5883-test.cpp bbb_i2c.cpp mpu6050.cpp timestamps.cpp hmc5883.cpp)

# gps
#       add_executable(test-uart-gps bbb-gps.cpp)
#target_link_libraries(test-uart-gps gps)

# pwm and motors
#       add_executable(test-pru-pwm-motors pru-motor-test.cpp pru.cpp pru-pwm-cpp.cpp)
#target_link_libraries(test-pru-pwm-motors prussdrv pthread)

#sixpair
#       add_executable(sixpair sixpair.c)
#target_link_libraries(sixpair usb)

# udp server
       add_executable(test-udp-json udpserver-test.cpp freeimu_linux.cpp bbb_i2c.cpp mpu6050.cpp timestamps.cpp hmc5883.cpp ms5611.cpp bbb-adc.cpp pru.cpp pru-pwm-cpp.cpp)
target_link_libraries(test-udp-json pthread jsoncpp prussdrv)

