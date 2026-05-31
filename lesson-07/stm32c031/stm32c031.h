//=================================================================
// This header file for STM32C031 MCU contains only the facilities
// used in the projects for the "Modern Embedded Programming" video
// course. This file is NOT intended to be complete.
//==================================================================

#ifndef STM32C031_H__
#define STM32C031_H__

#define RCC_IOPENR_R    (*((unsigned int *)0x40021034U))

#define GPIOA  ((volatile unsigned int *)0x50000000U)

#endif // STM32C031_H__
