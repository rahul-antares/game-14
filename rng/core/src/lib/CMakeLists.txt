cmake_minimum_required(VERSION 3.15)
project(rng)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(OpenSSL REQUIRED)

file(GLOB SRCS
     "../utils/*.cc"
     "../utils/*.h"
     "*.cc"
     "*.h"
)

#add_library(${PROJECT_NAME} SHARED ${SRCS})
#target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
#target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
#
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_LOG_ENABLED)
#endif()

# For Target "rng.node"
add_library(${PROJECT_NAME} SHARED ${SRCS} ${CMAKE_JS_SRC})
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_LOG_ENABLED)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -march=native -flto)
endif()

target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
execute_process(
    COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
)

string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})
add_definitions(-DNAPI_VERSION=3)

add_custom_target(create_symlink ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/lib/librng.so
    DEPENDS ${PROJECT_NAME}
    COMMENT "Creating symlink: librng.so → rng.node"
)

