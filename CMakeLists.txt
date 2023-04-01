cmake_minimum_required(VERSION 3.24)
project(NyanLang)

set(CMAKE_C_STANDARD 11)

add_executable(NyanLang main.c nyan.c nyan.h)
