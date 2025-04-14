cmake_minimum_required(VERSION 3.20)

project(WINSOCKLEARNING)

message("CMAKE_CXX_COMPILER:       " ${CMAKE_CXX_COMPILER})
message("CMAKE_CXX_COMPILER_ID:    " ${CMAKE_C_COMPILER_ID})
message("CMAKE_AR:                 " ${CMAKE_AR})
message("CMAKE_LINKER:             " ${CMAKE_LINKER})
message("CMAKE_BUILD_TYPE:         " ${CMAKE_BUILD_TYPE})
message("PROJECT_BINARY_DIR:       " ${PROJECT_BINARY_DIR})
message("PROJECT_SOURCE_DIR:       " ${PROJECT_SOURCE_DIR})
message("CMAKE_SYSTEM_NAME:        " ${CMAKE_SYSTEM_NAME})
message("CMAKE_SYSTEM_PROCESSOR:   " ${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID MATCH "MSVC")
    add_compile_options(/W3 /Wall /Za /WX)
else()
    add_compile_options(-W -Wall -Wextra -pedantic -pedantic-errors)
    add_link_options(-l ws2_32 mswsock)
endif()

set (SRC_LSIT_SEND
    send_test.cpp
    src/send.cpp
    src/common.cpp

)
set (SRC_LSIT_RECV
    recv_test.cpp
    src/recv.cpp
    src/common.cpp
)

add_executable(send_test ${SRC_LSIT_SEND})
add_executable(recv_test ${SRC_LSIT_RECV})