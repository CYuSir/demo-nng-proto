cmake_minimum_required(VERSION 3.10)
project(demo)

# Find required packages
find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

# Find nng package
find_package(nng REQUIRED)
message(STATUS "Using nng ${nng_VERSION}")

# Include directories
include_directories(include)

# Set output path to bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Add source files
set(CLIENT_SOURCES src/client.cpp)
set(SERVER_SOURCES src/server.cpp)

# Generate protobuf sources
protobuf_generate_cpp(CUSTOMS_PROTO_SRCS CUSTOMS_PROTO_HDRS proto/customs.proto)

# Define required libraries
set(REQUIRED_LIBS
    protobuf
    nng
    glog
    pthread
)

# Add executable targets
add_executable(client ${CLIENT_SOURCES} ${CUSTOMS_PROTO_SRCS} ${CUSTOMS_PROTO_HDRS})
add_executable(server ${SERVER_SOURCES} ${CUSTOMS_PROTO_SRCS} ${CUSTOMS_PROTO_HDRS})

# Link required libraries
target_link_libraries(client ${REQUIRED_LIBS})
target_link_libraries(server ${REQUIRED_LIBS})