# PowerDream Kernel File System Module

cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(kernel-fs)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_compile_definitions("INTERNAL_FS")
add_library(kernel-fs STATIC src/fs.c)
