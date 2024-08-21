cmake_minimum_required(VERSION 3.16)

project(bluefs
  VERSION 0.0.1
  LANGUAGES CXX C)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "This project requires Linux operating system.")
endif()

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Default BUILD_TYPE is Debug, other options are: RelWithDebInfo, Release, and MinSizeRel." FORCE)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
  fs/bluefs.cc
)

add_library(bluefs SHARED ${SOURCES})
set_target_properties(bluefs PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)