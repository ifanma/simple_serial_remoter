cmake_minimum_required(VERSION 2.8.0)
project(remo)

include(
    include
    )

link_directories(
    /opt/ros/indigo/lib
    )
add_executable(remo sbus_remoter.cpp)
target_link_libraries(remo serial m)
