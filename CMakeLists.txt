cmake_minimum_required(VERSION 2.4)

project(unix_socket_stream)

add_executable(unix_socket_stream
        src/logic.h
        src/app.h
        src/main.cpp
        src/logic.cpp
        src/app.cpp)

install(TARGETS unix_socket_stream DESTINATION bin)
