# Lesson 15 — Startup code на C (для STM32C031)

## Що нового порівняно з попередніми уроками

У lesson-04...lesson-13 ми використовували **асемблерний** startup (`startup_stm32c031c6.s`).
У цьому уроці він **переписаний на чистий C** — `startup_stm32c031xx.c`.

```
            попередньо                       тепер
            ─────────────                    ─────
            startup_stm32c031c6.s            startup_stm32c031xx.c
            (ARM Thumb assembly)             (стандартний C з атрибутами GCC)
```

Цим Samek доносить: **vector table і Reset_Handler можна (і простіше) писати на C**.
Жодних `.thumb_set`, `.section`, `.word` директив. Чистий C з кількома GCC-атрибутами.

> **Примітка:** Цей крок на STM32-треку ми вже зробили у [lesson-12](../../lesson-12/stm32c031/),
> коли взяли BSP-файли з `../nucleo-c031c6/gnu/`. Тут — самодостатня копія
> з фокусом саме на startup-файл.

## Як це працює

### 1. Vector table як C-масив

```c
__attribute__ ((section(".isr_vector")))    // ← покласти у спеціальну секцію
int const g_pfnVectors[] = {
    (int)&__stack_end__,                    // [0] initial SP (з linker'а)
    (int)&Reset_Handler,                    // [1] reset
    (int)&NMI_Handler,                      // [2]
    (int)&HardFault_Handler,                // [3]
    ...
};
```

Атрибут `section(".isr_vector")` каже linker'у покласти цей масив у секцію `.isr_vector`,
яку наш `.ld` script розміщує **на самому початку flash** — за адресою `0x08000000`.

### 2. Weak aliases через атрибут

```c
void NMI_Handler (void) __attribute__ ((weak));
void WWDG_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
```

- `weak` — символ можна перекрити власною реалізацією у будь-якому файлі
- `alias("Default_Handler")` — за замовчуванням вказує на `Default_Handler`

Якщо у `main.c` написати свою `void TIM3_IRQHandler(void) { ... }` — linker автоматично
перекриє weak alias твоєю функцією. **Без жодних змін у startup.**

### 3. Reset_Handler на C

```c
void Reset_Handler(void) {
    SystemInit();                                  // CMSIS системна ініціалізація

    // copy .data from flash to RAM
    unsigned const *src = &__data_load;
    unsigned *dst;
    for (dst = &__data_start; dst < &__data_end__; ++dst, ++src) {
        *dst = *src;
    }

    // zero-fill .bss
    for (dst = &__bss_start__; dst < &__bss_end__; ++dst) {
        *dst = 0;
    }

    __libc_init_array();                           // викликати C++ static-конструктори
    (void)main();                                  // нарешті — твій main()

    // якщо main() повернувся (а не повинен) — assert
    __asm volatile ("CPSID i");
    assert_failed("Reset_Handler", 1U);
}
```

Усі копіювання `.data` та зануляння `.bss` тепер у звичайних C-циклах `for`.
Жодного ASM — лише одна `__asm volatile` рядкова у самому кінці на випадок аномалії.

### 4. Fault handlers → controlled reset

```c
__attribute__((naked)) void HardFault_Handler(void) {
    __asm volatile ("CPSID i\n MOV sp,%0" : : "r" (&__stack_end__));
    assert_failed("HardFault", 1U);
}
```

- `__attribute__((naked))` — компілятор НЕ генерує пролог/епілог функції
  (без `push {lr}`, без `bx lr` у кінці). Це треба бо ми вручну керуємо стеком.
- Inline asm `CPSID i` — заборонити переривання
- `MOV sp,...` — скинути стек на знане місце (на випадок stack overflow)
- Потім — кличемо `assert_failed()` яка у `main.c` робить `NVIC_SystemReset()`.

Так fault не зависає процесор — а **переключає його у керований reset**.

## Що дає C-startup на практиці

| Аспект | ASM-startup | C-startup |
|--------|-------------|-----------|
| Читабельність | низька, треба знати ARM asm | висока, звичайний C |
| Розмір коду | мінімальний | трохи більший (зазвичай +50-100 байт) |
| Налагодження у debugger | step-by-step asm | step через C-цикли |
| Перекриття IRQ handler | `.weak`/`.thumb_set` | `__attribute__((weak))` |
| Vector table | `.word ...` | `int const g_pfnVectors[]` |
| Помилка → fault | endless loop | контрольований `NVIC_SystemReset()` |

## Як побудувати і запустити

```bash
# збилдити
cmake -S . -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# дебажити: F5 у VS Code
```

Поставити breakpoint у **Reset_Handler** і пройти **F10** — побачиш у Variables як
заповнюються `.data` і обнуляється `.bss` з C-коду. Це робить ту саму роботу що раніше
ASM-startup, тільки тепер **тобі видно у дебагері** кожен крок.

## Файли проекту

| Файл | Призначення |
|------|-------------|
| `main.c` | Той самий код що в lesson-13 (структури + init bytes) |
| `delay.c`, `delay.h` | Утиліта затримки |
| `startup_stm32c031xx.c` | **ГОЛОВНЕ** — startup на C. Vector table, Reset_Handler, fault handlers |
| `system_stm32c0xx.c` | CMSIS-функція `SystemInit()` (кличеться з Reset_Handler) |
| `nucleo-c031c6.ld` | Linker script з символами `__stack_end__`, `__data_load`, `__bss_start__`... |
| `CMakeLists.txt` | Білд-конфіг |
