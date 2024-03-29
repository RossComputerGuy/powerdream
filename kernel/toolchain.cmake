set(CMAKE_SYSTEM_PROCESSOR sh4)

set(CROSS_COMPILE kos-)

find_program(CMAKE_ASM_COMPILER ${CROSS_COMPILE}cc)
find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}cc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
