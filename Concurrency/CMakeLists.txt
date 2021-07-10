cmake_minimum_required(VERSION 3.19)
project(Concurrency)

set(CMAKE_CXX_STANDARD 20)

set(TARGET_LIST
        Concurrency
        Concurrency2
        Concurrency3
        Concurrency4
        Concurrency5
        Concurrency6
        Concurrency7
        Concurrency8
        Concurrency9
        Concurrency10
        )

add_executable(Concurrency main.cpp)
add_executable(Concurrency2 main2.cpp)
add_executable(Concurrency3 main3.cpp)
add_executable(Concurrency4 main4.cpp)
add_executable(Concurrency5 main5.cpp)
add_executable(Concurrency6 main6.cpp)
add_executable(Concurrency7 main7.cpp)
add_executable(Concurrency8 main8.cpp)
add_executable(Concurrency9 main9.cpp)
add_executable(Concurrency10 main10.cpp)

if (MSVC)
    # remove /W3 from defaults and set warning level per target later on
    string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "-W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# set warning levels
if (MSVC)
    foreach(target ${TARGET_LIST})
        target_compile_options(${target} PUBLIC /W4 /permissive)
    endforeach()
else()
    foreach(target ${TARGET_LIST})
        target_compile_options(${target} PUBLIC -Wall -Wextra -pedantic -pthread)
    endforeach()
endif()