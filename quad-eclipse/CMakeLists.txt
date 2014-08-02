
# http://www.cmake.org/cmake/help/cmake_tutorial.html

cmake_minimum_required (VERSION 2.6)

project (bbb-multicopter)

add_executable(bbb-gps bbb-gps.cpp)
target_link_libraries(bbb-gps gps)

add_executable(pru-motor-test pru-motor-test.cpp pru.cpp pruPWM.cpp)
target_link_libraries(pru-motor-test prussdrv pthread)

add_executable(ms5611-test ms5611-test.cpp)

add_executable(gy86 gy86.cpp)
target_link_libraries(gy86 m)



