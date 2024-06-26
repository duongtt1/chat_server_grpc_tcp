cmake_minimum_required(VERSION 3.8)

project(ChatServer C CXX)

include(./cmake/common.cmake)

# Proto file
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

# chat_grpc_proto
add_library(chat_grpc_proto
  ${chat_grpc_srcs}
  ${chat_grpc_hdrs}
  ${chat_proto_srcs}
  ${chat_proto_hdrs})
  
target_link_libraries(chat_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets [async_](client|server)
foreach(_target
  server)
  add_executable(${_target} "${_target}.cc" "SubRedisThread.cc" "ChatStreamImpl.cc")
  target_link_libraries(${_target}
    chat_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    -lhiredis)
endforeach()