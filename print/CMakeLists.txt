cmake_minimum_required(VERSION 3.0)
project(print)
set(SOURCE_LIB print.c)
add_library(print SHARED ${SOURCE_LIB})
