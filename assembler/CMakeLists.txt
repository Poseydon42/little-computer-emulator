cmake_minimum_required(VERSION 3.22)

project(assembler CXX)

set(SOURCES
    src/main.cpp
)

set(LIB_SOURCES
    src/CodeGenerator.cpp
    src/Lexer.cpp
    src/Parser.cpp
)

set(TEST_SOURCES
    tests/TestCodeGenerator.cpp
    tests/TestLexer.cpp
    tests/TestParser.cpp
)

add_library(libassembler STATIC ${LIB_SOURCES})
target_link_libraries(libassembler PUBLIC libcommon)
target_include_directories(libassembler PUBLIC include)

add_executable(assembler ${SOURCES})

target_link_libraries(assembler PRIVATE libassembler)

if(LCE_ENABLE_TESTS)
    add_executable(assembler-tests ${TEST_SOURCES})
    target_link_libraries(assembler-tests libassembler GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(assembler-tests)
endif()

