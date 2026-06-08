/**
 * Minimal startup for STM32C031C6 (Cortex-M0+)
 * - Builds vector table at 0x08000000
 * - Reset_Handler copies .data, zeros .bss, calls main()
 * - All unused IRQs default to an infinite loop (Default_Handler)
 */

    .syntax unified
    .cpu cortex-m0plus
    .fpu softvfp
    .thumb

    .global g_pfnVectors
    .global Default_Handler

/* Symbols defined by the linker script */
    .word _sidata           /* LMA of .data (in flash) */
    .word _sdata            /* VMA of .data start (in RAM) */
    .word _edata            /* VMA of .data end */
    .word _sbss             /* VMA of .bss start */
    .word _ebss             /* VMA of .bss end */

/* ---------------------------------------------------------------------- */
/*                              Reset_Handler                             */
/* ---------------------------------------------------------------------- */
    .section .text.Reset_Handler
    .weak  Reset_Handler
    .type  Reset_Handler, %function
Reset_Handler:
    ldr   r0, =_estack
    mov   sp, r0                /* set stack pointer (also in vector[0]) */

    /* Copy initialised data from flash (_sidata) to RAM (_sdata.._edata) */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
    movs  r3, #0
    b     LoopCopyDataInit

CopyDataInit:
    ldr   r4, [r2, r3]
    str   r4, [r0, r3]
    adds  r3, r3, #4

LoopCopyDataInit:
    adds  r4, r0, r3
    cmp   r4, r1
    bcc   CopyDataInit

    /* Zero-fill .bss (_sbss.._ebss) */
    ldr   r2, =_sbss
    ldr   r4, =_ebss
    movs  r3, #0
    b     LoopFillZerobss

FillZerobss:
    str   r3, [r2]
    adds  r2, r2, #4

LoopFillZerobss:
    cmp   r2, r4
    bcc   FillZerobss

    /* Run C++/C static constructors (no-op for plain C, but standard) */
    bl    __libc_init_array

    /* Jump to main() */
    bl    main

    /* main() should never return; trap here if it does */
LoopForever:
    b     LoopForever
    .size Reset_Handler, .-Reset_Handler

/* ---------------------------------------------------------------------- */
/*                            Default_Handler                             */
/* ---------------------------------------------------------------------- */
    .section .text.Default_Handler,"ax",%progbits
    .type  Default_Handler, %function
Default_Handler:
Infinite_Loop:
    b     Infinite_Loop
    .size Default_Handler, .-Default_Handler

/* ---------------------------------------------------------------------- */
/*                              Vector Table                              */
/* ---------------------------------------------------------------------- */
    .section .isr_vector,"a",%progbits
    .type   g_pfnVectors, %object
g_pfnVectors:
    .word _estack                     /* 0x00: Initial stack pointer        */
    .word Reset_Handler               /* 0x04: Reset                        */
    .word NMI_Handler                 /* 0x08: NMI                          */
    .word HardFault_Handler           /* 0x0C: Hard Fault                   */
    .word 0                           /* 0x10: reserved                     */
    .word 0                           /* 0x14: reserved                     */
    .word 0                           /* 0x18: reserved                     */
    .word 0                           /* 0x1C: reserved                     */
    .word 0                           /* 0x20: reserved                     */
    .word 0                           /* 0x24: reserved                     */
    .word 0                           /* 0x28: reserved                     */
    .word SVC_Handler                 /* 0x2C: SVCall                       */
    .word 0                           /* 0x30: reserved                     */
    .word 0                           /* 0x34: reserved                     */
    .word PendSV_Handler              /* 0x38: PendSV                       */
    .word SysTick_Handler             /* 0x3C: SysTick                      */

    /* STM32C031 external interrupts (32 slots) */
    .word WWDG_IRQHandler             /*  0 */
    .word PVD_IRQHandler              /*  1 */
    .word RTC_TAMP_IRQHandler         /*  2 */
    .word FLASH_IRQHandler            /*  3 */
    .word RCC_IRQHandler              /*  4 */
    .word EXTI0_1_IRQHandler          /*  5 */
    .word EXTI2_3_IRQHandler          /*  6 */
    .word EXTI4_15_IRQHandler         /*  7 */
    .word 0                           /*  8 reserved */
    .word DMA1_Channel1_IRQHandler    /*  9 */
    .word DMA1_Channel2_3_IRQHandler  /* 10 */
    .word DMAMUX1_IRQHandler          /* 11 */
    .word ADC1_IRQHandler             /* 12 */
    .word TIM1_BRK_UP_TRG_COM_IRQHandler /* 13 */
    .word TIM1_CC_IRQHandler          /* 14 */
    .word 0                           /* 15 reserved */
    .word TIM3_IRQHandler             /* 16 */
    .word 0                           /* 17 reserved */
    .word 0                           /* 18 reserved */
    .word TIM14_IRQHandler            /* 19 */
    .word 0                           /* 20 reserved */
    .word TIM16_IRQHandler            /* 21 */
    .word TIM17_IRQHandler            /* 22 */
    .word I2C1_IRQHandler             /* 23 */
    .word 0                           /* 24 reserved */
    .word SPI1_IRQHandler             /* 25 */
    .word 0                           /* 26 reserved */
    .word USART1_IRQHandler           /* 27 */
    .word USART2_IRQHandler           /* 28 */
    .word 0                           /* 29 reserved */
    .word 0                           /* 30 reserved */
    .word 0                           /* 31 reserved */

    .size g_pfnVectors, .-g_pfnVectors

/* ---------------------------------------------------------------------- */
/*                       Weak aliases for handlers                        */
/*  Every IRQ defaults to Default_Handler; user code can override any of  */
/*  them by defining a function with the same name.                       */
/* ---------------------------------------------------------------------- */
    .macro IRQ name
    .weak \name
    .thumb_set \name, Default_Handler
    .endm

    IRQ NMI_Handler
    IRQ HardFault_Handler
    IRQ SVC_Handler
    IRQ PendSV_Handler
    IRQ SysTick_Handler

    IRQ WWDG_IRQHandler
    IRQ PVD_IRQHandler
    IRQ RTC_TAMP_IRQHandler
    IRQ FLASH_IRQHandler
    IRQ RCC_IRQHandler
    IRQ EXTI0_1_IRQHandler
    IRQ EXTI2_3_IRQHandler
    IRQ EXTI4_15_IRQHandler
    IRQ DMA1_Channel1_IRQHandler
    IRQ DMA1_Channel2_3_IRQHandler
    IRQ DMAMUX1_IRQHandler
    IRQ ADC1_IRQHandler
    IRQ TIM1_BRK_UP_TRG_COM_IRQHandler
    IRQ TIM1_CC_IRQHandler
    IRQ TIM3_IRQHandler
    IRQ TIM14_IRQHandler
    IRQ TIM16_IRQHandler
    IRQ TIM17_IRQHandler
    IRQ I2C1_IRQHandler
    IRQ SPI1_IRQHandler
    IRQ USART1_IRQHandler
    IRQ USART2_IRQHandler
