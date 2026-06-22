cmake_minimum_required(VERSION 3.22)

project(lesson C ASM)
set(CMAKE_C_STANDARD 99)

# ---- Target MCU ---------------------------------------------------------
set(MCU_FLAGS
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
)

# ---- CMSIS — fully sourced from the repo-root vendor/ submodules --------
set(REPO_ROOT     ${CMAKE_SOURCE_DIR}/../..)
set(CMSIS_DEVICE  ${REPO_ROOT}/vendor/cmsis_device_c0/Include)
set(CMSIS_CORE    ${REPO_ROOT}/vendor/CMSIS_6/CMSIS/Core/Include)
# Official ST startup (GCC assembly) and system init, straight from vendor:
set(CMSIS_SRC     ${REPO_ROOT}/vendor/cmsis_device_c0/Source/Templates)

# ---- Sources ------------------------------------------------------------
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32C031C6Tx_FLASH.ld)

add_executable(${PROJECT_NAME}.elf
    main.c                                      # app + assert_failed
    bsp.c                                       # BSP + SysTick_Handler
    ${CMSIS_SRC}/gcc/startup_stm32c031xx.s      # ← vendor ST startup (vector table)
    ${CMSIS_SRC}/system_stm32c0xx.c             # ← vendor ST SystemInit()
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMSIS_DEVICE}
    ${CMSIS_CORE}
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    STM32C031xx
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -Wall -Wextra
    -ffunction-sections -fdata-sections
    $<$<CONFIG:Debug>:-g3 -O0>
    $<$<CONFIG:Release>:-O3>
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${PROJECT_NAME}.map
    -specs=nano.specs
    -specs=nosys.specs
)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    LINK_DEPENDS ${LINKER_SCRIPT}
)

# ---- Post-build: size + .hex/.bin/.lst ----------------------------------
set(TOOLCHAIN_DIR /opt/ST/STM32CubeCLT_1.21.0/GNU-tools-for-STM32/bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE}    $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${TOOLCHAIN_DIR}/arm-none-eabi-objdump -d -S -C
            $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Generating ${PROJECT_NAME}.hex, .bin and .lst"
    VERBATIM
)
