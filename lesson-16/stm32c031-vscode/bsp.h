/* Board Support Package — NUCLEO-C031C6 */
#ifndef BSP_H__
#define BSP_H__

/* Default system clock after reset on STM32C031C6:
 *   HSI48 (48 MHz) divided by SYSDIV=4 => HSISYS = 12 MHz
 * SysTick is clocked from the processor clock (HSISYS in our case).
 */
#define SYS_CLOCK_HZ  12000000U

/* On-board LED LD4 lives on PA5 */
#define LD4_PIN  5U

/* Optional external LED on D12 (PA6) — see lesson-05 wiring */
#define LD5_PIN  6U

#endif /* BSP_H__ */
