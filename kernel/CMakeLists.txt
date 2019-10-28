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

add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/initrd.img"
  COMMAND $ENV{KOS_GENROMFS} -f "${PROJECT_BINARY_DIR}/initrd.img" -d "${PROJECT_SOURCE_DIR}/initrd"
  COMMENT "Generating initrd image")
add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/initrd.o"
  COMMAND $ENV{KOS_BASE}/utils/bin2o/bin2o "${PROJECT_BINARY_DIR}/initrd.img" initrd "${PROJECT_BINARY_DIR}/initrd.o"
  COMMENT "Generating initrd object"
  DEPENDS "${PROJECT_BINARY_DIR}/initrd.img")
add_custom_target(initrd DEPENDS "${PROJECT_BINARY_DIR}/initrd.o")

set(FILESYSTEMS "${PROJECT_SOURCE_DIR}/fs/initrd.c")

add_compile_options("-Werror=implicit-function-declaration")
add_executable(kernel
  ${FILESYSTEMS}
  "${PROJECT_BINARY_DIR}/initrd.o"
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
