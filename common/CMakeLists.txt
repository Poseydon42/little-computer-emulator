cmake_minimum_required(VERSION 3.22)

project(common)

add_library(libcommon INTERFACE)

target_include_directories(libcommon INTERFACE include)
