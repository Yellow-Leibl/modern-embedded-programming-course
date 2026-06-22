/* Lesson-17 — observing interrupt preemption on STM32 NUCLEO-C031C6
 *
 * main() pulses the GREEN-analog pin (PA6) as fast as the CPU can.
 * The SysTick ISR toggles the RED-analog pin (PA5) every ~0.5 s.
 *
 * On a logic analyzer you can SEE that whenever SysTick fires, the
 * dense stream of GREEN pulses briefly stops — the CPU has been
 * pulled away from main() to run SysTick_Handler(). That gap is the
 * interrupt latency + ISR execution time, measurable with cursors.
 */
#include "stm32c031xx.h"
#include "bsp.h"

int main(void) {
    /* Enable GPIOA clock */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    /* Configure PA5 (RED) and PA6 (GREEN) as push-pull outputs */
    GPIOA->MODER   &= ~((3U << 2U*LED_RED_PIN) | (3U << 2U*LED_GREEN_PIN));
    GPIOA->MODER   |=  ((1U << 2U*LED_RED_PIN) | (1U << 2U*LED_GREEN_PIN));
    GPIOA->OTYPER  &= ~((1U <<    LED_RED_PIN) | (1U <<    LED_GREEN_PIN));
    GPIOA->OSPEEDR &= ~((3U << 2U*LED_RED_PIN) | (3U << 2U*LED_GREEN_PIN));
    GPIOA->PUPDR   &= ~((3U << 2U*LED_RED_PIN) | (3U << 2U*LED_GREEN_PIN));

    /* SysTick: interrupt every SYS_CLOCK_HZ/2 cycles (~0.5 s) */
    SysTick->LOAD = SYS_CLOCK_HZ / 2U - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                  | SysTick_CTRL_TICKINT_Msk
                  | SysTick_CTRL_ENABLE_Msk;

    /* Call the ISR once by hand — proof that an ISR is just a C
     * function. This sets the initial RED state before IRQs start. */
    SysTick_Handler();

    __enable_irq();

    /* Foreground "task": pulse GREEN as fast as possible */
    while (1) {
        GPIOA->BSRR = (1U << LED_GREEN_PIN);          /* GREEN on  */
        GPIOA->BSRR = (1U << (LED_GREEN_PIN + 16U));  /* GREEN off */
    }
    /* unreachable */
}
