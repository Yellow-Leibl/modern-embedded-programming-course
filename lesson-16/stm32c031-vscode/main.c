/* Lesson-16 — first interrupt: SysTick on STM32 NUCLEO-C031C6
 *
 * Compare to lesson-13: there the main loop busy-waited inside delay().
 * Here main() is empty — the LED toggles ON ITS OWN, driven by the
 * SysTick interrupt running in the background. This is a glimpse of
 * how real embedded software is structured.
 */
#include "stm32c031xx.h"   /* CMSIS device header */
#include "bsp.h"

int main(void) {
    /* Enable GPIOA clock so the port is alive */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    /* Configure PA5 as push-pull output, low speed, no pull-up/down */
    GPIOA->MODER   &= ~(3U << 2U * LD4_PIN);
    GPIOA->MODER   |=  (1U << 2U * LD4_PIN);
    GPIOA->OTYPER  &= ~(1U <<      LD4_PIN);
    GPIOA->OSPEEDR &= ~(3U << 2U * LD4_PIN);
    GPIOA->PUPDR   &= ~(3U << 2U * LD4_PIN);

    /* SysTick setup: fire an interrupt every SYS_CLOCK_HZ/2 cycles.
     *   LOAD : count-down reload value (24-bit). At HSISYS=12 MHz,
     *          12_000_000 / 2 - 1 = 5_999_999 — interrupt every 0.5 s.
     *   VAL  : current counter — reset to 0 (will reload on next tick).
     *   CTRL : ENABLE | TICKINT | CLKSOURCE
     *          bit 0 (ENABLE)    — start the counter
     *          bit 1 (TICKINT)   — generate interrupt on reaching 0
     *          bit 2 (CLKSOURCE) — use processor clock (not external)
     */
    SysTick->LOAD = SYS_CLOCK_HZ / 2U - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                  | SysTick_CTRL_TICKINT_Msk
                  | SysTick_CTRL_ENABLE_Msk;

    /* Globally enable IRQs (clears the PRIMASK bit) */
    __enable_irq();

    /* THE WHOLE POINT: main() does NOTHING from here on.
     * LD4 is being toggled by SysTick_Handler() in the background. */
    while (1) {
    }
    /* unreachable */
}
