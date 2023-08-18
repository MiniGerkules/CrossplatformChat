cmake_minimum_required(VERSION 3.20)
project(Client LANGUAGES CXX)

set(SOURCES
    src/main.cpp
)
set(HEADERS
    ./include/Client.hpp

    ./include/WrongMessageException.hpp

    ./include/Displayer/Displayer.hpp
    ./include/Displayer/ConsoleDisplayer.hpp
    ./include/Displayer/DisplayerException.hpp

    ./include/Reader/Reader.hpp
    ./include/Reader/ReaderDelegate.hpp
    ./include/Reader/ConsoleReader.hpp

    ./include/CheckResponders/AppCheckResponder.hpp
    ./include/CheckResponders/ModAppCheckResponder.hpp
    ./include/CheckResponders/CreateResponseException.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

set_compile_options_to(${PROJECT_NAME})
add_project_libs_to(${PROJECT_NAME})
add_boost_to(${PROJECT_NAME})
