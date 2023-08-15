cmake_minimum_required(VERSION 3.20)
project(CrossplatformChat)

option(BUILD_TESTS "Turn on if your want to build and run all tests" OFF)

find_package(Boost REQUIRED COMPONENTS system)

set(LIB_NAME LibChat)

include(${CMAKE_SOURCE_DIR}/functions.cmake)

add_subdirectory(libchat)
add_subdirectory(client)
add_subdirectory(server)
