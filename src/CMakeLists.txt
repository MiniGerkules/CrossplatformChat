cmake_minimum_required(VERSION 3.12)
set(NAME CrosspatformChat)

project(${NAME} LANGUAGES CXX)

set(SOURCES
    main.cpp
    Helpers.cpp
)
set(HEADERS
    LaunchOptions.hpp
    Client.hpp
    Server.hpp
    Helpers.hpp
    Message.hpp
    TSQueue.hpp
    Connection.hpp
    Runnable.hpp
    PossibleIDs.hpp
)

add_executable(${NAME} ${SOURCES} ${HEADERS})

if (WIN32)
    target_compile_options(${NAME} PRIVATE /std:c++17 /W4)
else()
    target_compile_options(${NAME} PRIVATE -std=c++17 -Wall -Wextra -Wpedantic -Waddress)
endif()

# Add main lib to use
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../asio/include)

# Add dependencies
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../system/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../config/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../throw_exception/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../assert/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../winapi/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../predef/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../date_time/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../smart_ptr/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../core/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../utility/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../static_assert/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../type_traits/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../numeric_conversion/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../mpl/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../preprocessor/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../bind/include)
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../regex/include)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_custom_command(TARGET ${NAME} POST_BUILD COMMAND 
    ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install
)

install(FILES $<TARGET_FILE:${NAME}> DESTINATION ${CMAKE_BINARY_DIR}/bin)
