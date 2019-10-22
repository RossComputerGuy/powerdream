# Toolchain builder

cmake_minimum_required(VERSION 3.15)
project(pd-toolchain NONE)

configure_file("${PROJECT_SOURCE_DIR}/environ.sh.in" "${PROJECT_BINARY_DIR}/environ.sh" @ONLY)

execute_process(COMMAND sh download.sh
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/kos/utils/dc-chain")
execute_process(COMMAND sh unpack.sh
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/kos/utils/dc-chain")

add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/sh4-elf"
  COMMAND make erase=1 sh_prefix=${PROJECT_BINARY_DIR}/sh4-elf arm_prefix=${PROJECT_BINARY_DIR}/arm-eabi
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/kos/utils/dc-chain")
add_custom_target(dc-chain DEPENDS "${PROJECT_BINARY_DIR}/sh4-elf")