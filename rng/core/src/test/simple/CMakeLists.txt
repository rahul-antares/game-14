cmake_minimum_required(VERSION 3.15)
project(simple-test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)

find_package(OpenSSL REQUIRED)

file(GLOB SRCS
     "../../api/*.h"
     "main.cc"
)

add_executable(${PROJECT_NAME} ${SRCS})
target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto ${LIB_DIR}/librngapi-local.so)
add_dependencies(${PROJECT_NAME} rngapi-local)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_LOG_ENABLED)
endif()

set(RNG_VERSION rng-version)
file(GLOB VERSION_SRCS
     "../../api/*.h"
     "rng-version.cc"
)

add_executable(${RNG_VERSION} ${VERSION_SRCS})
target_compile_options(${RNG_VERSION} PUBLIC -fexceptions)
target_link_libraries(${RNG_VERSION} OpenSSL::SSL OpenSSL::Crypto ${LIB_DIR}/librngapi-local.so)
add_dependencies(${RNG_VERSION} rngapi-local)
