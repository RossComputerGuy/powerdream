# PowerDream libc

cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(pd-libc C ASM)
set(PROJECT_VERSION "0.1.0")

include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(c STATIC
  "${PROJECT_SOURCE_DIR}/src/sys/syscall.c"
  "${PROJECT_SOURCE_DIR}/src/lib.c"
  "${PROJECT_SOURCE_DIR}/src/string.c")
