# PowerDream Kernel

cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(pd-kernel)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${PROJECT_SOURCE_DIR}/fs")
add_executable(kernel "${PROJECT_SOURCE_DIR}/src/boot.c" "${PROJECT_SOURCE_DIR}/src/inode.c" "${PROJECT_SOURCE_DIR}/src/process.c" "${PROJECT_SOURCE_DIR}/src/syscall.c")
add_dependencies(kernel kernel-fs)
target_link_libraries(kernel kernel-fs)
