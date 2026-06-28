/* Board Support Package (BSP) for the NUCLEO-C031C6 board */
#include <stdint.h>  /* Standard integers. WG14/N843 C99 Standard */

#include "bsp.h"
#include "miros.h"
#include "stm32c0xx.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */

// LED marked "LD4" (PA.5) on the NUCLEO-C031C6 board
#define LD4_PIN  5U

// "blue" LED — external LED on D12 (PA.6); also a probe point for the LA
#define LD5_PIN  6U

// "red" LED — external LED / probe point on D11 (PA.7)
#define LD6_PIN  7U

// Button B1 (PC.13) on the NUCLEO-C031C6 board
#define B1_PIN   13U

static uint32_t volatile l_tickCtr;

void SysTick_Handler(void) {
    ++l_tickCtr;

    __disable_irq();
    OS_sched();
    __enable_irq();
}

void BSP_init(void) {
    // enable GPIOA clock port for the LEDs
    RCC->IOPENR |= (1U << 0U);

    // NUCLEO-C031C6 board has LED LD4 on GPIOA pin LD4_PIN
    // and external LED LD5 on GPIO LD5_PIN
    // set the LED pins as push-pull output, no pull-up, pull-down
    GPIOA->MODER   &= ~((3U << 2U*LD4_PIN) | (3U << 2U*LD5_PIN) | (3U << 2U*LD6_PIN));
    GPIOA->MODER   |=  ((1U << 2U*LD4_PIN) | (1U << 2U*LD5_PIN) | (1U << 2U*LD6_PIN));
    GPIOA->OTYPER  &= ~((1U <<    LD4_PIN) | (1U <<    LD5_PIN) | (1U <<    LD6_PIN));
    GPIOA->OSPEEDR &= ~((3U << 2U*LD4_PIN) | (3U << 2U*LD5_PIN) | (3U << 2U*LD6_PIN));
    GPIOA->OSPEEDR |=  ((1U << 2U*LD4_PIN) | (1U << 2U*LD5_PIN) | (1U << 2U*LD6_PIN));
    GPIOA->PUPDR   &= ~((3U << 2U*LD4_PIN) | (3U << 2U*LD5_PIN) | (3U << 2U*LD6_PIN));

    // configure Button B1 (PC.13) pins as input, no pull-up, pull-down
    GPIOC->MODER   &= ~(3U << 2*B1_PIN);
    GPIOC->OSPEEDR &= ~(3U << 2*B1_PIN);
    GPIOC->OSPEEDR |=  (1U << 2*B1_PIN);
    GPIOC->PUPDR   &= ~(3U << 2*B1_PIN);
}

uint32_t BSP_tickCtr(void) {
    uint32_t tickCtr;

    __disable_irq();
    tickCtr = l_tickCtr;
    __enable_irq();

    return tickCtr;
}

void BSP_delay(uint32_t ticks) {
    uint32_t start = BSP_tickCtr();
    while ((BSP_tickCtr() - start) < ticks) {
    }
}

void BSP_ledRedOn(void) {
    GPIOA->BSRR = (1U << LD6_PIN);          /* PA7 (D11) high */
}

void BSP_ledRedOff(void) {
    GPIOA->BSRR = (1U << (LD6_PIN + 16U));  /* PA7 (D11) low  */
}

void BSP_ledBlueOn(void) {
    GPIOA->BSRR = (1U << LD5_PIN);          /* PA6 (D12) high */
}

void BSP_ledBlueOff(void) {
    GPIOA->BSRR = (1U << (LD5_PIN + 16U));  /* PA6 (D12) low  */
}

void BSP_ledGreenOn(void) {
    GPIOA->BSRR = (1U << LD4_PIN);  /* turn LED on  */
}

void BSP_ledGreenOff(void) {
    GPIOA->BSRR = (1U << (LD4_PIN + 16U));  /* turn LED off */
}

void OS_onStartup(void) {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set the SysTick interrupt priority (highest) */
    NVIC_SetPriority(SysTick_IRQn, 0U);
}

void OS_onIdle(void) {
#ifdef NDBEBUG
    __WFI(); /* stop the CPU and Wait for Interrupt */
#endif
}

void Q_onAssert(char const *module, int id) {
    /* TBD: damage control */
    (void)module; /* avoid the "unused parameter" compiler warning */
    (void)id;     /* avoid the "unused parameter" compiler warning */
    NVIC_SystemReset();
}

void assert_failed(char const *module, int id) {
    Q_onAssert(module, id);
}

