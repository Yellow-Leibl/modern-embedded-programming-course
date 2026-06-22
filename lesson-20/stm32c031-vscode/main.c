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
    SysTick->LOAD = SYS_CLOCK_HZ / 2U - 1U; /* counts from LOAD down to 0 */
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
        GPIOA->ODR |= (1U << LED_GREEN_PIN);    // set GREEN on
        GPIOA->ODR &= ~(1U << LED_GREEN_PIN);   // set GREEN off
    }
    /* unreachable */
}
