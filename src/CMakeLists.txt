cmake_minimum_required(VERSION 3.12)
set(NAME CrosspatformChat)

project(${NAME} LANGUAGES CXX)

set(SOURCES
    main.cpp
    Client.cpp
    Server.cpp
    CMDArgsHelpers.cpp
    NetworkingMember.cpp
)
set(HEADERS
    LaunchOption.hpp
    Client.hpp
    Server.hpp
    CMDArgsHelpers.hpp
    NetworkingMember.hpp
)

add_executable(${NAME} ${SOURCES} ${HEADERS})

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
