# PowerDream

cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(powerdream)

add_subdirectory(kernel)

add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/IP.BIN"
  COMMAND dc-makeip "${PROJECT_SOURCE_DIR}/data/ip.txt" "${PROJECT_BINARY_DIR}/IP.BIN"
  COMMENT "Generating IP.BIN")
