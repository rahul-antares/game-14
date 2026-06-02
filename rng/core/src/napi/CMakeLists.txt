cmake_minimum_required(VERSION 3.15)
project(rng-module)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(${CMAKE_JS_INC})
file(GLOB GAME_SRCS 
     "../lib/*.cc" 
     "../lib/*.h" 
     "../utils/*.cc" 
     "../utils/*.h" 
     "*.cc"
     "*.h")

add_library(${PROJECT_NAME} SHARED ${GAME_SRCS} ${CMAKE_JS_SRC})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_LOG_ENABLED)
else()
    target_compile_options(${PROJECT_NAME} PUBLIC -O3 -march=native -flto -DNDEBUG)
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
