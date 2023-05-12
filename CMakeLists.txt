cmake_minimum_required(VERSION 3.10)
project(demo)

# Find required packages
find_package(Protobuf REQUIRED)

# Include directories
include_directories(include)

# Add source files
set(CLIENT_SOURCES src/client.cpp)
set(SERVER_SOURCES src/server.cpp)

# Generate protobuf sources
protobuf_generate_cpp(CUSTOMS_PROTO_SRCS CUSTOMS_PROTO_HDRS proto/customs.proto)

# Add executable targets
add_executable(client ${CLIENT_SOURCES} ${CUSTOMS_PROTO_SRCS} ${CUSTOMS_PROTO_HDRS})
add_executable(server ${SERVER_SOURCES} ${CUSTOMS_PROTO_SRCS} ${CUSTOMS_PROTO_HDRS})

# Link required libraries
target_link_libraries(client protobuf nng)
target_link_libraries(server protobuf nng)