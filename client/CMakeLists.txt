cmake_minimum_required(VERSION 3.20)
project(Client LANGUAGES CXX)

add_executable(${PROJECT_NAME} src/main.cpp)

set_compile_options_to(${PROJECT_NAME})
add_project_libs_to(${PROJECT_NAME})
add_boost_to(${PROJECT_NAME})
