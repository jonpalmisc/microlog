cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(microlog LANGUAGES C)

option(MICROLOG_BUILD_EXAMPLE "Build example program" OFF)

add_library(microlog SHARED microlog.h microlog.c)
target_compile_features(microlog PRIVATE c_std_99)
target_include_directories(microlog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MICROLOG_BUILD_EXAMPLE)
  add_executable(example example.c)
  target_link_libraries(example PRIVATE microlog)
endif()
