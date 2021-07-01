cmake_minimum_required(VERSION 3.16)
project(synsensecollection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

file(GLOB DIR_SRCS *.cpp)
list(REMOVE_ITEM DIR_SRCS "main.cpp")

add_library(synsensecollection SHARED ${DIR_SRCS})

add_executable(test main.cpp)
target_link_libraries(test ${PROJECT_NAME})
target_link_libraries(test pthread)