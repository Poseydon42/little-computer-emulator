cmake_minimum_required(VERSION 3.22)

project(assembler CXX)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
)

add_executable(assembler ${SOURCES})
target_include_directories(assembler PUBLIC include)
