cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(MiniLog LANGUAGES C)

add_library(MiniLog SHARED MiniLog.h MiniLog.c)
target_compile_features(MiniLog PRIVATE c_std_99)
target_include_directories(MiniLog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
