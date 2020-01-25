cmake_minimum_required(VERSION 3.15)
project(myfs C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(myfs
        myfs.c logger.h)
