# PowerDream Kernel

cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(pd-kernel C ASM)
set(PROJECT_VERSION "0.1.0")

set(PD_SERIAL_BAUD 57600)
set(PD_SERIAL_FIFO 1)

configure_file("${PROJECT_SOURCE_DIR}/data/config.h.in" "${PROJECT_BINARY_DIR}/kernel-config.h")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${PROJECT_SOURCE_DIR}/fs")
add_compile_options("-nostdinc" "-Werror=implicit-function-declaration")
add_link_options("-nostartfiles" "-nostdlib")
add_executable(kernel
  "${PROJECT_SOURCE_DIR}/src/asm/cache.s"
  "${PROJECT_SOURCE_DIR}/src/asm/entry.s"
  "${PROJECT_SOURCE_DIR}/src/asm/irq.s"
  "${PROJECT_SOURCE_DIR}/src/asm/mmu.s"
  "${PROJECT_SOURCE_DIR}/src/dev/block.c"
  "${PROJECT_SOURCE_DIR}/src/dev/char.c"
  "${PROJECT_SOURCE_DIR}/src/dev/tty.c"
  "${PROJECT_SOURCE_DIR}/src/boot.c"
  "${PROJECT_SOURCE_DIR}/src/device.c"
  "${PROJECT_SOURCE_DIR}/src/fs.c"
  "${PROJECT_SOURCE_DIR}/src/inode.c"
  "${PROJECT_SOURCE_DIR}/src/irq.c"
  "${PROJECT_SOURCE_DIR}/src/mem.c"
  "${PROJECT_SOURCE_DIR}/src/mmu.c"
  "${PROJECT_SOURCE_DIR}/src/process.c"
  "${PROJECT_SOURCE_DIR}/src/sem.c"
  "${PROJECT_SOURCE_DIR}/src/sercon.c"
  "${PROJECT_SOURCE_DIR}/src/string.c"
  "${PROJECT_SOURCE_DIR}/src/syscall.c"
  "${PROJECT_SOURCE_DIR}/src/system.c")
#add_dependencies(kernel kernel-fs)
#target_link_libraries(kernel kernel-fs)
