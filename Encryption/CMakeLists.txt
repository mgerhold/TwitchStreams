cmake_minimum_required(VERSION 3.8)

project("Encryption")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Server "server.cpp" "net.h")
add_executable(Client "client.cpp" "net.h")

