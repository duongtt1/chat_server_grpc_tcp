cmake_minimum_required(VERSION 3.10)
project(ServerSample)

include(./cmake/common.cmake)

# Set C++ standard and compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)

# Compiler flags
set(CMAKE_CXX_FLAGS "-std=c++2a -Wall -Wextra -Wpedantic -Wno-unused-variable -lhiredis")

# Enable verbose makefile for more detailed output
set(CMAKE_VERBOSE_MAKEFILE on)

get_filename_component(chat_proto "./protos/chat.proto" ABSOLUTE)
get_filename_component(chat_proto_path "${chat_proto}" PATH)

# Generated sources
set(chat_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.cc")
set(chat_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.h")
set(chat_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.cc")
set(chat_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.h")
add_custom_command(
      OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}" "${chat_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${chat_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${chat_proto}"
      DEPENDS "${chat_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${Protobuf_INCLUDE_DIRS})

add_library(chat_grpc_proto
  ${chat_grpc_srcs}
  ${chat_grpc_hdrs}
  ${chat_proto_srcs}
  ${chat_proto_hdrs})
  
target_link_libraries(chat_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Add source files
set(SOURCES
    Server.cpp
    socket/tcp_server.cpp
    socket/tcp_socket.cpp
    utils/CacheThread.cpp
    protocols/request_handler.cpp
    # db/groupchatdb.cpp
)

# Add header files
set(HEADERS
    Server.h
    protocols/generate/factory_request.h
    protocols/generate/protocol_utils.h
    protocols/generate/request_handler.h
    protocols/generate/request_template.h
    protocols/generate/response_template.h
    protocols/generate/types.h
    protocols/interfaces/RequestBase.h
    protocols/interfaces/ResponseBase.h
    socket/tcp_server.h
    socket/tcp_socket.h
    utils/macros.h
    utils/socket_utils.h
    utils/thread_utils.h
    utils/SafeQueue.h
    utils/CacheThread.h
    # db/groupchatdb.h
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/protocols/
    ${PROJECT_SOURCE_DIR}/protocols/generate
    ${PROJECT_SOURCE_DIR}/protocols/interfaces
    ${PROJECT_SOURCE_DIR}/utils
)

# Targets [async_](client|server)
foreach(_target
  sender main)
  add_executable(${_target} "${_target}.cpp" ${SOURCES})
  target_link_libraries(${_target}
    chat_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    -lhiredis
    )
endforeach()

