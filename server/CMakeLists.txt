cmake_minimum_required(VERSION 3.20)
project(Server LANGUAGES CXX)

set(SOURCES
    ./src/main.cpp
)
set(HEADERS
    ./include/Server.hpp

    ./include/ClientsManager/ClientsManager.hpp
    ./include/ClientsManager/ClientsManagerDelegate.hpp

    ./include/ClientsValidator/ClientsValidator.hpp
    ./include/ClientsValidator/ClientsValidatorDelegate.hpp

    ./include/ClientsValidator/CheckCreater/AppCheckCreater.hpp
    ./include/ClientsValidator/CheckCreater/NumAppCheckCreater.hpp

    ./include/ClientsValidator/CheckInspector/AppCheckInspector.hpp
    ./include/ClientsValidator/CheckInspector/ModAppCheckInspector.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

set_compile_options_to(${PROJECT_NAME})
add_project_libs_to(${PROJECT_NAME})
add_boost_to(${PROJECT_NAME})
