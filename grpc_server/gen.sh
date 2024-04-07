#!/bin/bash
protoc -I=./ --cpp_out=./generate protos/chat.proto
protoc -I=./ --grpc_out=./generate --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` protos/chat.proto