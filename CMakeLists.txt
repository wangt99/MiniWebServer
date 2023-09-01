cmake_minimum_required(VERSION 3.20)
project(
  miniweb
  VERSION 0.0.1
  LANGUAGES C CXX
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

message("compiling on the OS of ${CMAKE_SYSTEM_NAME}")

add_subdirectory(src bin)
add_subdirectory(test)
add_subdirectory(examples)

