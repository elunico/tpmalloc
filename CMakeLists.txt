cmake_minimum_required(VERSION 3.16)
project(tpmalloc C)

set(CMAKE_C_FLAGS "-O0 -g")
set(CMAKE_C_STANDARD 11)

add_library(tpmalloc tpsupport.h tpsupport.c malloc.c malloc.h)
add_executable(test main.c tpsupport.c tpsupport.h malloc.c malloc.h)
