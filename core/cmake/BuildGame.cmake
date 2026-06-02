#
# common function to build game library
#

function(BUILD_GAME_LIBRARY target variant)
    target_include_directories(${target} PRIVATE ${TOP_DIR}/../rng/core/src/api/ ../game/)
    target_link_libraries(${target} PRIVATE ${TOP_DIR}/../rng/core/build/lib/librngapi-local.so)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        #
        # uncomment in case of heap corruption
        #
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        target_compile_definitions(${target} PRIVATE DEBUG_LOG_ENABLED)
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
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

    set(SYMLINKLOCAL ${CMAKE_BINARY_DIR}/lib/lib${target}.so)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${target}.node
                ${SYMLINKLOCAL}
                COMMENT "Creating symlink: ${SYMLINKLOCAL} → ${target}.node"
    )

endfunction()

