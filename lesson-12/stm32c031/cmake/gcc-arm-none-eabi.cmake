# Toolchain file for the ARM GCC shipped with STM32CubeCLT.
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake ..

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Don't try to link a normal executable in CMake's compiler tests
# (linker script and startup are not available yet during the probe).
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TOOLCHAIN_DIR    /opt/ST/STM32CubeCLT_1.21.0/GNU-tools-for-STM32/bin)
set(TOOLCHAIN_PREFIX ${TOOLCHAIN_DIR}/arm-none-eabi-)

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "")
set(CMAKE_SIZE         ${TOOLCHAIN_PREFIX}size    CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
