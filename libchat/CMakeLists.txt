cmake_minimum_required(VERSION 3.20)
project(${LIB_NAME} LANGUAGES CXX)

set(SOURCES
    ./src/HeartbeatChecker.cpp
)
set(HEADERS
    ./include/Message.hpp
    ./include/HeartbeatChecker.hpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_compile_options_to(${PROJECT_NAME})
add_boost_to(${PROJECT_NAME})
