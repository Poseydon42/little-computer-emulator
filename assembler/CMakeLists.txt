cmake_minimum_required(VERSION 3.22)

project(assembler CXX)

set(SOURCES
    src/main.cpp
)

add_executable(assembler ${SOURCES})
target_include_directories(assembler PUBLIC include)
