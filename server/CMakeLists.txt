cmake_minimum_required(VERSION 3.20)
project(Server LANGUAGES CXX)

add_executable(${PROJECT_NAME} src/main.cpp)

set_compile_options_to(${PROJECT_NAME})
add_libraries_to(${PROJECT_NAME})
