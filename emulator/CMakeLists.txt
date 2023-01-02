cmake_minimum_required(VERSION 3.22)

project(emulator CXX)

set(LIB_SOURCES
    src/CPU.cpp
    src/RandomAccessMemoryBlock.cpp
)

set(TEST_SOURCES
    tests/TestCPU.cpp
)

add_library(libemulator STATIC ${LIB_SOURCES})
target_link_libraries(libemulator PUBLIC libassembler libcommon)
target_include_directories(libemulator PUBLIC include)

if(LCE_ENABLE_TESTS)
    add_executable(emulator-tests ${TEST_SOURCES})
    target_link_libraries(emulator-tests libemulator GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(emulator-tests)
endif()
