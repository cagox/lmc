cmake_minimum_required(VERSION 3.20)
project(lmc C)

set(CMAKE_C_STANDARD 11)

add_executable(lmc main.c lmc.h)
