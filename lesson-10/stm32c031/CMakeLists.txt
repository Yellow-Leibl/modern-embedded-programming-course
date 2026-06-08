cmake_minimum_required(VERSION 3.22)

project(lesson C ASM)
set(CMAKE_C_STANDARD 99)

# ---- Target MCU ---------------------------------------------------------
set(MCU_FLAGS
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
)

# ---- CMSIS (submodules in repo-root vendor/) ----------------------------
set(REPO_ROOT     ${CMAKE_SOURCE_DIR}/../..)
set(CMSIS_DEVICE  ${REPO_ROOT}/vendor/cmsis_device_c0/Include)
set(CMSIS_CORE    ${REPO_ROOT}/vendor/CMSIS_6/CMSIS/Core/Include)

# ---- Shared sources (no main here) --------------------------------------
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32C031C6Tx_FLASH.ld)
set(COMMON_SOURCES
    delay.c
    startup_stm32c031c6.s
)

set(TOOLCHAIN_DIR /opt/ST/STM32CubeCLT_1.21.0/GNU-tools-for-STM32/bin)

# ---- Helper: produce one firmware variant per main_*.c ------------------
# Usage: add_lesson_variant(<name> <path/to/main.c>)
#   creates <name>.elf, <name>.hex, <name>.bin, <name>.lst, <name>.map
function(add_lesson_variant variant_name main_src)
    set(elf ${variant_name}.elf)

    add_executable(${elf} ${main_src} ${COMMON_SOURCES})

    target_include_directories(${elf} PRIVATE
        ${CMSIS_DEVICE}
        ${CMSIS_CORE}
    )

    target_compile_definitions(${elf} PRIVATE
        STM32C031xx
    )

    target_compile_options(${elf} PRIVATE
        ${MCU_FLAGS}
        -Wall -Wextra
        -ffunction-sections -fdata-sections
        $<$<CONFIG:Debug>:-g3 -O0>
        $<$<CONFIG:Release>:-O3>
    )

    target_link_options(${elf} PRIVATE
        ${MCU_FLAGS}
        -T${LINKER_SCRIPT}
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        -Wl,-Map=${variant_name}.map
        -specs=nano.specs
        -specs=nosys.specs
    )

    set_target_properties(${elf} PROPERTIES
        LINK_DEPENDS ${LINKER_SCRIPT}
    )

    add_custom_command(TARGET ${elf} POST_BUILD
        COMMAND ${CMAKE_SIZE}    $<TARGET_FILE:${elf}>
        COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${elf}> ${variant_name}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${elf}> ${variant_name}.bin
        COMMAND ${TOOLCHAIN_DIR}/arm-none-eabi-objdump -d -S -C
                $<TARGET_FILE:${elf}> > ${variant_name}.lst
        COMMENT "Generating ${variant_name}.hex, .bin and .lst"
        VERBATIM
    )
endfunction()

# ---- Lesson variants ----------------------------------------------------
add_lesson_variant(lesson       main.c)
add_lesson_variant(lesson_frac  main_frac.c)
add_lesson_variant(lesson_swap  main_swap.c)
