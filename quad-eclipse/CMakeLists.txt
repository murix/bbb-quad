
# http://www.cmake.org/cmake/help/cmake_tutorial.html

cmake_minimum_required (VERSION 2.6)

project (bbb-multicopter)

# barometer
add_executable(ms5611-test  ms5611-test.cpp  bbb_i2c.cpp ms5611.cpp )

# accelerometer and gyroscope
add_executable(mpu6050-test mpu6050-test.cpp bbb_i2c.cpp mpu6050.cpp)

# magnetometer
add_executable(hmc5883-test hmc5883-test.cpp bbb_i2c.cpp mpu6050.cpp hmc5883.cpp)

# gps
add_executable(bbb-gps bbb-gps.cpp)
target_link_libraries(bbb-gps gps)

# pwm and motors
add_executable(pru-motor-test pru-motor-test.cpp pru.cpp pruPWM.cpp)
target_link_libraries(pru-motor-test prussdrv pthread)


#add_executable(gy86 gy86.cpp)
#target_link_libraries(gy86 m pthread jsoncpp)



