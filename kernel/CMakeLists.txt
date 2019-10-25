# PowerDream Kernel

cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(pd-kernel C ASM)
set(PROJECT_VERSION "0.1.0")

option(COMPILED_CMDLINE "Compile the kernel command line options into the binary instead of loading from a file" OFF)

if (COMPILED_CMDLINE)
  if (NOT CMDLINE)
    set(CMDLINE "")
  endif()
endif()

set(CMAKE_USER "$ENV{USER}")
configure_file("${PROJECT_SOURCE_DIR}/data/config.h.in" "${PROJECT_BINARY_DIR}/kernel-config.h")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")

set(FILESYSTEMS)

add_compile_options("-Werror=implicit-function-declaration")
add_executable(kernel
  ${FILESYSTEMS}
  "${PROJECT_SOURCE_DIR}/src/dev/block.c"
  "${PROJECT_SOURCE_DIR}/src/dev/char.c"
  "${PROJECT_SOURCE_DIR}/src/dev/tty.c"
  "${PROJECT_SOURCE_DIR}/src/fs/devfs.c"
  "${PROJECT_SOURCE_DIR}/src/fs/procfs.c"
  "${PROJECT_SOURCE_DIR}/src/boot.c"
  "${PROJECT_SOURCE_DIR}/src/device.c"
  "${PROJECT_SOURCE_DIR}/src/fs.c"
  "${PROJECT_SOURCE_DIR}/src/inode.c"
  "${PROJECT_SOURCE_DIR}/src/klog.c"
  "${PROJECT_SOURCE_DIR}/src/module.c"
  "${PROJECT_SOURCE_DIR}/src/process.c"
  "${PROJECT_SOURCE_DIR}/src/syscall.c"
  "${PROJECT_SOURCE_DIR}/src/system.c")
