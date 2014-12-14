
# http://www.cmake.org/cmake/help/cmake_tutorial.html

cmake_minimum_required (VERSION 2.6)

project (bbb-multicopter)

# cc1101 spi tap
add_executable(test-cc1101-tap cc1101-tap.cpp)

# vbat
add_executable(test-vbat bbb-adc.cpp bbb-adc-test.cpp)

# barometer
add_executable(test-ms5611  ms5611-test.cpp  bbb_i2c.cpp ms5611.cpp )

# accelerometer and gyroscope
add_executable(test-mpu6050 mpu6050-test.cpp bbb_i2c.cpp mpu6050.cpp timestamps.cpp)

# magnetometer
add_executable(test-hmc5883 hmc5883-test.cpp bbb_i2c.cpp mpu6050.cpp timestamps.cpp hmc5883.cpp)

# gps
add_executable(test-gps bbb-gps.cpp)
target_link_libraries(test-gps gps)

# pwm and motors
add_executable(test-motors pru-motor-test.cpp pru.cpp pruPWM.cpp)
target_link_libraries(test-motors prussdrv pthread)

# udp server
       add_executable(test-udpjson udpserver-test.cpp udpserver.cpp bbb_i2c.cpp mpu6050.cpp timestamps.cpp hmc5883.cpp ms5611.cpp)
target_link_libraries(test-udpjson pthread jsoncpp)

