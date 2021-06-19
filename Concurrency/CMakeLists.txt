cmake_minimum_required(VERSION 3.19)
project(Concurrency)

set(CMAKE_CXX_STANDARD 20)

add_executable(Concurrency main.cpp)
add_executable(Concurrency2 main2.cpp)

if (MSVC)
    # remove /W3 from defaults and set warning level per target later on
    string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "-W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# set warning levels
if (MSVC)
    target_compile_options(Concurrency PUBLIC /W4 /permissive)
    target_compile_options(Concurrency2 PUBLIC /W4 /permissive)
else()
    target_compile_options(Concurrency PUBLIC -Wall -Wextra -pedantic)
    target_compile_options(Concurrency2 PUBLIC -Wall -Wextra -pedantic)
endif()