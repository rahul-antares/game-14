cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(game)

##########################################################################################
# Common directives
##########################################################################################

include_directories(${CMAKE_JS_INC})
file(GLOB GAME_SRCS 
    "../../../rng/core/src/api/rng.h"
    "../utils/*.cc" 
    "*.cc"
    "*.h")

##########################################################################################
# RTP Version 96
##########################################################################################

set(GAME96 game96)
add_library(${GAME96} SHARED ${GAME_SRCS} ${PARENT_GAME_SRCS} ${CMAKE_JS_SRC})
BUILD_GAME_LIBRARY(${GAME96} VARIANT_96)

##########################################################################################
# RTP Version 94
##########################################################################################

set(GAME94 game94)
add_library(${GAME94} SHARED ${GAME_SRCS} ${PARENT_GAME_SRCS} ${CMAKE_JS_SRC})
BUILD_GAME_LIBRARY(${GAME94} VARIANT_94)


##########################################################################################
# RTP Version 92
##########################################################################################

set(GAME92 game92)
add_library(${GAME92} SHARED ${GAME_SRCS} ${PARENT_GAME_SRCS} ${CMAKE_JS_SRC})
BUILD_GAME_LIBRARY(${GAME92} VARIANT_92)
