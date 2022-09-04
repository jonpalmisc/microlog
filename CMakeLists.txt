cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(microlog LANGUAGES C)

add_library(microlog SHARED microlog.h microlog.c)
target_compile_features(microlog PRIVATE c_std_99)
target_include_directories(microlog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
