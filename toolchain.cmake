set(CMAKE_SYSTEM_PROCESSOR sh4)

find_program(CMAKE_C_COMPILER kos-cc)
find_program(CMAKE_CXX_COMPILER kos-c++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)