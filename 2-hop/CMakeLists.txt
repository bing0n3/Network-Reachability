cmake_minimum_required(VERSION 3.16)
project(2_hop)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(2_hop
    2-hop-labeling.py
    2-hop-labels.cpp
    2-hop-labels.h
    a.out
    datafile.txt
        bechmark.cpp)
