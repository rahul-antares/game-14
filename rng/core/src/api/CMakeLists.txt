cmake_minimum_required(VERSION 3.15)
project(rngapi)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(OpenSSL REQUIRED)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_LOG_ENABLED)
else()
    add_compile_definitions(NDEBUG)
    add_compile_options(-O3 -march=native -flto)
endif()

add_compile_options(-fexceptions)

#########################################################
#
# Make librngapi-grpc.so
#
#########################################################

set(LIBGRPC "${PROJECT_NAME}-grpc")

# Find Protobuf and gRPC
find_package(Protobuf REQUIRED)
#find_package(gRPC REQUIRED)

# Locate protoc and grpc_cpp_plugin manually
find_program(PROTOC_EXECUTABLE NAMES protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin REQUIRED)

# Define .proto files
set(PROTO_FILES ${TOP_DIR}/../service/grpc/rng.proto)

# Define generated files
set(GENERATED_SRCS "${CMAKE_CURRENT_BINARY_DIR}/rng.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/rng.grpc.pb.cc")
set(GENERATED_HDRS "${CMAKE_CURRENT_BINARY_DIR}/rng.pb.h" "${CMAKE_CURRENT_BINARY_DIR}/rng.grpc.pb.h")

# Run protoc with the correct paths
add_custom_command(
    OUTPUT ${GENERATED_SRCS} ${GENERATED_HDRS}
    COMMAND ${PROTOC_EXECUTABLE}  # Use absolute path for protoc
    ARGS --proto_path=${TOP_DIR}/../service/grpc
         --grpc_out=${CMAKE_CURRENT_BINARY_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}  # Use absolute path for grpc_cpp_plugin
         ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
)

file(GLOB LIBGRPC_SRCS
     "../utils/*.cc"
     "../utils/*.h"
     "rng-grpc.cc"
     "rng-grpc.h"
     "rng.cc"
     "rng.h"
     ${GENERATED_SRCS}
)

add_library(${LIBGRPC} SHARED ${LIBGRPC_SRCS} ${GENERATED_SRCS} ${GENERATED_HDRS})

# Include generated files
target_sources(${LIBGRPC} PRIVATE rng-grpc.cc)
target_include_directories(${LIBGRPC} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../service/grpc)
target_compile_definitions(${LIBGRPC} PRIVATE __RNG__USE_GRPC__)
# Link necessary libraries
target_link_libraries(${LIBGRPC}
    #gRPC::grpc++
    #protobuf::libprotobuf
    protobuf grpc++ grpc gpr libabsl_synchronization.so.20210324
    #absl::synchronization
)

#########################################################
#
# Make librngapi-nats.so
#
#########################################################
set(LIBNATS "${PROJECT_NAME}-nats")
file(GLOB LIBNATS_SRCS
     "../utils/*.cc"
     "../utils/*.h"
     "rng-nats.cc"
     "rng-nats.h"
     "rng.cc"
     "rng.h"
)

# Find the NATS library
find_library(NATS_LIB nats REQUIRED)
find_path(NATS_INCLUDE_DIR nats/nats.h REQUIRED)

add_library(${LIBNATS} SHARED ${LIBNATS_SRCS})
target_sources(${LIBNATS} PRIVATE rng-nats.cc)
target_compile_definitions(${LIBNATS} PRIVATE __RNG__USE_NATS__)
target_include_directories(${LIBNATS} PRIVATE ${NATS_INCLUDE_DIR})
target_link_libraries(${LIBNATS} PRIVATE ${NATS_LIB})

#########################################################
#
# Make librngapi-local.so
#
#########################################################
set(LIBLOCAL "${PROJECT_NAME}-local")
file(GLOB LIBLOCAL_SRC
     "../utils/*.cc"
     "../utils/*.h"
     "../lib/date.cc"
     "../lib/date.h"
     "../lib/rand.cc"
     "../lib/rand.h"
     "rng-local.cc"
     "rng-local.h"
     "rng.cc"
     "rng.h"
)

add_library(${LIBLOCAL} SHARED ${LIBLOCAL_SRC})
target_sources(${LIBLOCAL} PRIVATE rng-local.cc)
target_compile_definitions(${LIBLOCAL} PRIVATE __RNG__USE_LOCAL__)
target_link_libraries(${LIBLOCAL} OpenSSL::SSL OpenSSL::Crypto)

add_custom_target(create_symlink_rngpi ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    $<TARGET_FILE:${LIBLOCAL}> ${CMAKE_BINARY_DIR}/lib/librngapi.so
    DEPENDS ${LIBLOCAL}
    COMMENT "Creating symlink: ${PROJECT_NAME} → ${CMAKE_BINARY_DIR}/lib/librngapi.so"
)

