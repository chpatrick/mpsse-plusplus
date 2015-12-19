cmake_minimum_required(VERSION 3.3)
project(mpsseplusplus)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(mpsseplusplus MPSSE.cpp MPSSE.h)
target_link_libraries(mpsseplusplus mpsse)