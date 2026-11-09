cmake_minimum_required(VERSION 4.3)
project(device_sim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(device_sim main.cpp device.cpp)
add_executable(device_sim_tests tests.cpp device.cpp)

enable_testing()
add_test(NAME device_model COMMAND device_sim_tests)