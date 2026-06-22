# Modern Embedded Systems Programming — STM32 / Cross-Platform Fork

This repository is a **personal fork / adaptation** of the excellent
[**"Modern Embedded Systems Programming" video course**](https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM)
created by **Miro Samek** ([Quantum Leaps, LLC](https://www.state-machine.com)).

All the teaching, lesson structure, and original source code belong to Miro Samek.
If you are here to learn, start with the original course — watch the videos and
read the class notes:

- 📺 Course playlist: <https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM>
- 🌐 Class notes & downloads: <https://www.state-machine.com/quickstart>
- 📦 Upstream repository: <https://github.com/QuantumLeaps/modern-embedded-programming-course>

Please support the original author — subscribe to the
[Quantum Leaps YouTube channel](https://www.youtube.com/@StateMachineCOM)
and star the upstream repository.

---

## What this fork changes

The original course ships its STM32 projects for **Keil µVision** and **IAR EWARM**,
both of which are **Windows-only** and license-restricted. This fork replaces them
with a **fully cross-platform, open-toolchain workflow** that runs natively on
**macOS (Apple Silicon), Linux, and Windows** — using only free tooling:

| Layer | Original course | This fork |
|-------|-----------------|-----------|
| IDE | Keil µVision / IAR (Windows) | **VS Code** (any OS) |
| Compiler | ARMCC / IAR | **arm-none-eabi-gcc** (from STM32CubeCLT) |
| Build system | IDE project files | **CMake + Ninja** |
| Debugger | Keil/IAR built-in | **Cortex-Debug + ST-Link GDB Server** |
| Board support | RTE / CMSIS-Pack | **CMSIS as git submodules** + hand-written startup/linker |

The target board is the **STM32 NUCLEO-C031C6** (ARM Cortex-M0+), the same board
the original course supports.

## Toolchain (what you need to install)

1. **STM32CubeCLT** — bundles `arm-none-eabi-gcc`, GDB, CMake, Ninja,
   ST-Link GDB Server, and `STM32_Programmer_CLI`.
   <https://www.st.com/en/development-tools/stm32cubeclt.html>
2. **st-stlink-server** package (ships alongside CubeCLT on macOS).
3. **VS Code** with extensions:
   - `marus25.cortex-debug` — debugging
   - `ms-vscode.cmake-tools` — build integration
   - `llvm-vs-code-extensions.vscode-clangd` — IntelliSense

> ⚠️ The build/debug config files (`CMakeLists.txt`, `.vscode/*`,
> `cmake/gcc-arm-none-eabi.cmake`) contain **absolute paths** to
> `STM32CubeCLT_1.21.0`. Adjust the version/path if yours differs.

## Repository layout

```
modern-embedded-programming-course/
├── vendor/                      ← CMSIS as git submodules (shared by all lessons)
│   ├── cmsis_device_c0/         ← STMicroelectronics CMSIS device headers
│   └── CMSIS_6/                 ← ARM CMSIS Core (Cortex-M0+ intrinsics)
│
├── lesson-04/stm32c031/         ← cross-platform project per lesson
│   ├── main.c                   ← lesson source (from the original course)
│   ├── startup_stm32c031*.s/.c  ← startup (vector table, Reset_Handler)
│   ├── *.ld                     ← GNU linker script
│   ├── CMakeLists.txt           ← build config
│   ├── cmake/                   ← GCC toolchain file
│   └── .vscode/                 ← build + debug tasks
│
└── lesson-NN/...                ← original Keil/IAR/CCS folders are left intact
```

## Cloning (submodules!)

CMSIS lives in git submodules, so clone recursively:

```bash
git clone --recurse-submodules <this-repo-url>
# or, if already cloned:
git submodule update --init --recursive
```

## Building & debugging a lesson

```bash
cd lesson-13/stm32c031     # (or lesson-NN/stm32c031-vscode for later lessons)

cmake -S . -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

In VS Code: open the lesson folder, press **F5** to build → flash → debug
(stops at `main`). The build also emits `lesson.hex`, `lesson.bin`, a `.map`
file, and a `.lst` (mixed C/assembly disassembly) under `build/`.

## Adapted lessons

| Lesson | Folder | Topic |
|--------|--------|-------|
| 04 | `lesson-04/stm32c031` | GPIO, blink an LED (raw register addresses) |
| 05 | `lesson-05/stm32c031` | external LED on D12 |
| 06 | `lesson-06/stm32c031` | bitwise operators |
| 07 | `lesson-07/stm32c031` | arrays / register access |
| 08 | `lesson-08/stm32c031` | — |
| 09 | `lesson-09/stm32c031` | CMSIS device header |
| 10 | `lesson-10/stm32c031` | functions, the call stack (multi-variant build) |
| 11 | `lesson-11/stm32c031` | type casting pitfalls |
| 12 | `lesson-12/stm32c031` | structures, CMSIS-style register access |
| 13 | `lesson-13/stm32c031` | `.data` / `.bss`, init bytes in the linker map |
| 15 | `lesson-15/stm32c031-vscode` | startup code written in C |
| 16 | `lesson-16/stm32c031-vscode` | first interrupt (SysTick) |
| 17 | `lesson-17/stm32c031-vscode` | observing interrupt preemption on a logic analyzer |
| 18 | `lesson-18/stm32c031-vscode` | interrupt entry/exit, the exception stack frame |
| 20 | `lesson-20/stm32c031-vscode` | race conditions (visible on a logic analyzer) |
| 21 | `lesson-21/stm32c031-vscode` | — |

> Notes:
> - **Lesson 14** (intentionally broken startup) is left as the original
>   demo — do not flash it.
> - **Lesson 19** (TI Code Composer Studio) is TivaC-specific and not adapted.
> - Folder naming is inconsistent (`stm32c031` vs `stm32c031-vscode`) for
>   historical reasons within this fork.

## Notes specific to Cortex-M0+ (STM32C031)

A few course topics differ from the TivaC (Cortex-M4F) track:

- **No bit-banding** on Cortex-M0+ — the array-style GPIO access from the
  TivaC lesson-07 doesn't apply; register offsets are used instead.
- **No FPU** — the FPU stack-frame part of lesson-18 doesn't apply.
- The hardware-stacking exception model (lessons 16–18) is identical to all
  Cortex-M cores.

## Hardware

- **STM32 NUCLEO-C031C6** development board (on-board ST-Link debugger).
- A cheap **USB logic analyzer** (FX2-based, 8-channel) is very useful from
  lesson-17 onward to observe GPIO timing, interrupt preemption, and race
  conditions. Works with `sigrok-cli` / PulseView on macOS.

---

## License & attribution

The course content, lesson structure, and original source code are
**© Quantum Leaps, LLC** and Miro Samek, under their respective licenses
(see upstream). This fork only adds build/debug scaffolding (CMake, VS Code
config, GNU startup/linker files) to make the lessons buildable with an
open, cross-platform toolchain. All credit for the teaching goes to the
original author.
