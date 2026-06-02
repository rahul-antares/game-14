cmake_minimum_required(VERSION 3.15)
project(c++harness)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

file(GLOB GAME_SRCS 
     "../../game/*.cc"
     "../../game/*.h"
     "../../utils/*.cc"
     "../../utils/*.h"
)
list(FILTER GAME_SRCS EXCLUDE REGEX "main")
list(FILTER GAME_SRCS EXCLUDE REGEX "data-conversion")
include_directories(${CMAKE_JS_INC})
file(GLOB DRIVER_SRCS 
     "../harness.h"
     "../harness.cc"
     "*.h"
     "*.cc"
)

add_executable(${PROJECT_NAME} ${DRIVER_SRCS} ${GAME_SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE ${TOP_DIR}/../rng/core/src/api/)
target_link_libraries(${PROJECT_NAME} PRIVATE ${TOP_DIR}/../rng/core/build/lib/librngapi-local.so)

#
# Add the profiler flag
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_LOG_ENABLED)
endif()
