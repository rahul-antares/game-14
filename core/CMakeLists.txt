cmake_minimum_required(VERSION 3.15)
project(AllGameVariants)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(TOP_DIR ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(BuildGame)

#include(FetchContent)
#
#FetchContent_Declare(
#    rng_api
#    SOURCE_DIR ../../../rng/core/src/api
#)
#
#FetchContent_MakeAvailable(rng_api)

add_subdirectory(src/game)
add_subdirectory(src/game-wc)
add_subdirectory(src/test)
# add_subdirectory(src/test/c++harness)
# add_subdirectory(src/test/c++driver)
