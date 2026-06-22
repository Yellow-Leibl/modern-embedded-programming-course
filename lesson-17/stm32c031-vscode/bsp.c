/* Board Support Package — NUCLEO-C031C6 */
#include "stm32c031xx.h"
#include "bsp.h"

/* SysTick ISR — toggles the RED-analog LED (PA5) every ~0.5 s.
 * Cortex-M0+ has a hardware stacking feature that automatically saves the CPU registers on the stack
 * when an interrupt occurs. This allows the ISR to be written in C, just like any other function.
 * The compiler generates the prologue and epilogue code to save and restore the registers.
 * On other processors, the ISR might need to be written in assembly language to save and restore
 * the registers manually.
 */
void SysTick_Handler(void) {
    GPIOA->ODR ^= (1U << LED_RED_PIN);
}

/* Called by the Samek C-startup on any fault → controlled reset. */
_Noreturn void assert_failed(char const * const module, int const id);
_Noreturn void assert_failed(char const * const module, int const id) {
    (void)module;
    (void)id;
    NVIC_SystemReset();
    for (;;) { }
}
