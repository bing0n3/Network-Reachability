cmake_minimum_required(VERSION 3.16)
project(landmark)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(landmark
        LandmarkLabeling.cpp
        LandmarkLabeling.h benchmark.cpp)
