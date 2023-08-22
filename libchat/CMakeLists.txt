cmake_minimum_required(VERSION 3.20)
project(${LIB_NAME} LANGUAGES CXX)

set(SOURCES
    ./src/phony.cpp
)
set(HEADERS
    ./include/Consts.hpp

    ./include/BaseException.hpp

    ./include/TSQueue.hpp

    ./include/VectorExtensions.hpp

    ./include/Connection/Connection.hpp
    ./include/Connection/TCPConnection.hpp
    ./include/Connection/ConnectionManager.hpp

    ./include/Connection/Delegates/ConnectionDelegate.hpp
    ./include/Connection/Delegates/ConnectionManagerDelegate.hpp

    ./include/Functions/ModFunction.hpp

    ./include/Logger/Logger.hpp
    ./include/Logger/ConsoleLogger.hpp
    ./include/Logger/LoggerMessageType.hpp

    ./include/Messages/Message.hpp
    ./include/Messages/UniversalMessage.hpp

    ./include/Messages/Handlers/MessageHandler.hpp
    ./include/Messages/Handlers/BasicMessageHandler.hpp
    ./include/Messages/Handlers/RegularMessageHandler.hpp
    ./include/Messages/Handlers/ConnectionMessageHandler.hpp

    ./include/Messages/Handlers/Delegates/BasicMessageHandlerDelegate.hpp
    ./include/Messages/Handlers/Delegates/RegularMessageHandlerDelegate.hpp
    ./include/Messages/Handlers/Delegates/ConnectionMessageHandlerDelegate.hpp

    ./include/Messages/Types/BasicMessageType.hpp
    ./include/Messages/Types/RegularMessageType.hpp
    ./include/Messages/Types/ConnectionMessageType.hpp

    ./include/Messages/Types/MessageTypesFuncs.hpp
    ./include/Messages/Types/DescriptionException.hpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

set_compile_options_to(${PROJECT_NAME})
add_boost_to(${PROJECT_NAME})

if (${BUILD_TESTS})
    add_subdirectory(tests)
endif()
