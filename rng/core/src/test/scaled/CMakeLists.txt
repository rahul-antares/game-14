cmake_minimum_required(VERSION 3.15)
project(scaled)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)

find_package(OpenSSL REQUIRED)

file(GLOB SRCS
     "../../utils/*.h"
     "../../lib/*.h"
     "sizes.cc"
     "main.cc"
)

add_executable(${PROJECT_NAME} ${SRCS})
target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto ${LIB_DIR}/librngapi-local.so)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_LOG_ENABLED)
endif()
