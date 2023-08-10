cmake_minimum_required(VERSION 3.11)

project(Lesson4 CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(negate main.cpp img_lib.h img_lib.cpp ppm_image.h ppm_image.cpp)