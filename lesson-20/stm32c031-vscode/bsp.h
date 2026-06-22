/* Board Support Package — NUCLEO-C031C6 */
#ifndef BSP_H__
#define BSP_H__

/* HSI48 / SYSDIV(4) = 12 MHz processor clock after reset */
#define SYS_CLOCK_HZ  12000000U

/* RED-analog: on-board LED LD4 on PA5, toggled by the SysTick ISR  */
#define LED_RED_PIN    5U

/* GREEN-analog: external LED / probe point on D12 (PA6),
 * pulsed as fast as possible by the main() foreground loop          */
#define LED_GREEN_PIN  6U

/* SysTick ISR — also called by hand once from main() (see lesson) */
void SysTick_Handler(void);

#endif /* BSP_H__ */
