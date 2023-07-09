# Function set all needed compile options to target
function(set_compile_options_to target)
    # Set c++17 standart
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)

    # Set warning flags
    if (MSVC)
        target_compile_options(${target} PRIVATE /Wall /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)

        # If it's the debug mode, turn on sanitizers
        if (${CMAKE_BUILD_TYPE} EQUAL Debug)
            target_compile_options(${target} PRIVATE -fsanitize=address,memory_leaks)
        endif()
    endif()
endfunction()

function(add_project_libs_to target)
    # Include project library
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/libchat/include)
    target_link_libraries(${target} PRIVATE ${LIB_NAME})
endfunction()

function(add_boost_to target)
    # Include Boost
    target_include_directories(${target} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${Boost_LIBRARIES})
endfunction()
