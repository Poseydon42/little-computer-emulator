cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LCE_ENABLE_TESTS "Build and run tests for LCE (requires GTest)" OFF)

if (LCE_ENABLE_TESTS)
    message("Tests are enabled")
    enable_testing()

    set(gtest_force_shared_crt ON CACHE BOOL "Force GTest to link the C runtime library as a dynamic library" FORCE)

    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 15460959cbbfa20e66ef0b5ab497367e47fc0a04
    )

    FetchContent_MakeAvailable(googletest)
endif()

project(lce)

add_subdirectory(assembler)
add_subdirectory(common)
add_subdirectory(emulator)
