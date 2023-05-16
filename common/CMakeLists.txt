cmake_minimum_required(VERSION 3.22)

project(common)

include(FetchContent)
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(thirdparty/cxxopts)

set(SOURCES
    src/ErrorReporting.cpp
)

add_library(libcommon STATIC ${SOURCES})

target_include_directories(libcommon PUBLIC include)
target_link_libraries(libcommon PUBLIC cxxopts fmt)
