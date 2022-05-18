cmake_minimum_required(VERSION 3.16)
project(tpmalloc C)

set(CMAKE_C_FLAGS "-O0 -g")
set(CMAKE_C_STANDARD 11)

add_executable(tpmalloc main.c tpsupport.h tpsupport.c malloc.c malloc.h)
