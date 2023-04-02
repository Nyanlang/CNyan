cmake_minimum_required(VERSION 3.24)
project(NyanLang)

set(CMAKE_C_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter -Wno-unused-variable")

add_executable(NyanLang main.c nyan.c nyan.h mouse.h mouse.c util.c util.h argparse.c argparse.h)
