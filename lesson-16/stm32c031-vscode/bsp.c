/* Board Support Package — NUCLEO-C031C6 */
#include "stm32c031xx.h"   /* CMSIS device header */
#include "bsp.h"

/* ---------------------------------------------------------------------
 * SysTick ISR — fires every SYS_CLOCK_HZ/2 cycles (≈ 500 ms at 12 MHz).
 * Each fire toggles LD4 via XOR on the BSRR register.
 *
 * NOTE: BSRR is write-only (no bit can be both set and reset in one
 * write — you choose one). We can't XOR BSRR itself. Instead we read
 * ODR (output data register), XOR it, and write back via ODR. ODR is
 * R/W and reflects current pin state.
 * ------------------------------------------------------------------ */
void SysTick_Handler(void) {
    GPIOA->ODR ^= (1U << LD4_PIN);
}

/* ---------------------------------------------------------------------
 * assert_failed() — called by the Samek C-startup on any fault.
 * Performs a controlled system reset instead of locking up.
 * ------------------------------------------------------------------ */
_Noreturn void assert_failed(char const * const module, int const id);
_Noreturn void assert_failed(char const * const module, int const id) {
    (void)module;
    (void)id;
    NVIC_SystemReset();
    for (;;) { }
}
