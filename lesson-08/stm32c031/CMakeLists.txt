cmake_minimum_required(VERSION 3.22)

project(lesson C ASM)
set(CMAKE_C_STANDARD 99)

# ---- Target MCU ---------------------------------------------------------
set(MCU_FLAGS
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
)

# ---- Sources ------------------------------------------------------------
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32C031C6Tx_FLASH.ld)

add_executable(${PROJECT_NAME}.elf
    main.c
    startup_stm32c031c6.s
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
    $<$<CONFIG:Debug>:-g3 -Os>
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

# Rebuild if the linker script changes
set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    LINK_DEPENDS ${LINKER_SCRIPT}
)

# ---- Post-build: print size + emit .hex/.bin + .lst (mixed C/asm) -------
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
