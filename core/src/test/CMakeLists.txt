cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

######################################################################
# common function to build harness
######################################################################

function(BUILD_HARNESS target variant gamelib)
    include_directories(${CMAKE_JS_INC} ../../../rng/core/src/api)
    add_library(${target} SHARED ${HARNESS_SRCS} ${GAME_SRCS} ${CMAKE_JS_SRC})
    target_link_libraries(${target} PRIVATE ${gamelib})

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        #
        # uncomment in case of heap corruption
        #
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        target_compile_definitions(${target} PRIVATE DEBUG_LOG_ENABLED)
    endif()

    target_compile_options(${target} PUBLIC -fexceptions)
    target_compile_definitions(${target} PRIVATE ${variant})
    set_target_properties(${target} PROPERTIES PREFIX "" SUFFIX ".node")
    target_link_libraries(${target} ${CMAKE_JS_LIB})
    execute_process(
        COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
    )

    string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
    string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
    target_include_directories(${target} PRIVATE ${NODE_ADDON_API_DIR})
    add_definitions(-DNAPI_VERSION=3)
endfunction()

######################################################################
# End of function
######################################################################

project(harness96)

file(GLOB HARNESS_SRCS 
    "*.h"
    "*.cc"
)

BUILD_HARNESS(harness96 VARIANT_96 game96)
BUILD_HARNESS(harness94 VARIANT_94 game94)
BUILD_HARNESS(harness92 VARIANT_92 game92)
