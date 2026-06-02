cmake_minimum_required(VERSION 3.15)
project(Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_LOG_ENABLED)
else()
    add_compile_definitions(NDEBUG)
    add_compile_options(-O3 -march=native -flto)
endif()

# Find Protobuf and gRPC
find_package(Protobuf REQUIRED)
#find_package(gRPC REQUIRED)

# Locate protoc and grpc_cpp_plugin manually
find_program(PROTOC_EXECUTABLE NAMES protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin REQUIRED)

# Define .proto files
set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/rng.proto)

# Define generated files
set(GENERATED_SRCS "${CMAKE_CURRENT_BINARY_DIR}/rng.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/rng.grpc.pb.cc")
set(GENERATED_HDRS "${CMAKE_CURRENT_BINARY_DIR}/rng.pb.h" "${CMAKE_CURRENT_BINARY_DIR}/rng.grpc.pb.h")

# Run protoc with the correct paths
add_custom_command(
    OUTPUT ${GENERATED_SRCS} ${GENERATED_HDRS}
    COMMAND ${PROTOC_EXECUTABLE}  # Use absolute path for protoc
    ARGS --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
         --grpc_out=${CMAKE_CURRENT_BINARY_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}  # Use absolute path for grpc_cpp_plugin
         ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
)

# Server Source Files
set(SERVER_SOURCES
    server.cc
    ${GENERATED_SRCS}
)

# Create the server executable
add_executable(server ${SERVER_SOURCES})

# Include generated files
target_include_directories(server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_directories(server PRIVATE ${CMAKE_BINARY_DIR}/../../../core/build/lib)

# Link necessary libraries
target_link_libraries(server
    protobuf grpc++ grpc gpr libabsl_synchronization.so.20210324
    #gRPC::grpc++
    #protobuf::libprotobuf
    pthread
    #absl::synchronization
    rng
)

#
# create test client
#
set(CLIENT_SOURCES
    test/client.cc
    ${GENERATED_SRCS}
)

set(CLIENT client)
add_executable(${CLIENT} ${CLIENT_SOURCES})

# Include generated files
target_include_directories(${CLIENT} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)
#target_link_directories(server PRIVATE ${CMAKE_BINARY_DIR}/../../../core/build/lib)

# Link necessary libraries
target_link_libraries(${CLIENT}
    protobuf grpc++ grpc gpr libabsl_synchronization.so.20210324
    #gRPC::grpc++
    #protobuf::libprotobuf
    pthread
    #absl::synchronization
)

#
# create load client
#
set(LOAD_SOURCES
    test/load.cc
    ${GENERATED_SRCS}
)

set(LOAD load)
add_executable(${LOAD} ${LOAD_SOURCES})

# Include generated files
target_include_directories(${LOAD} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)
#target_link_directories(server PRIVATE ${CMAKE_BINARY_DIR}/../../../core/build/lib)

# Link necessary libraries
target_link_libraries(${LOAD}
    protobuf grpc++ grpc gpr libabsl_synchronization.so.20210324
    #gRPC::grpc++
    #protobuf::libprotobuf
    pthread
    #absl::synchronization
)
