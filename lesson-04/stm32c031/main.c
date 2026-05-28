// NOTE:
// NUCLEO-C031C6 registers require using the binary operators
// such as '|', '&', and '~' that will be explained in lesson-06

int main(void) {

    // enable GPIOA clock port for the LED LD4
    // RCC + iopenr offset : 0x40021000 + 0x34 = 0x4002 1034
    *((unsigned int *)0x40021034U) |= 0x01U; // RCC IOPENR register

    // NUCLEO-C031C6 board has user LED LD4 on GPIOA pin 5
    // set the LED pin as push-pull output, no pull-up, pull-down
    *((unsigned int *)0x50000000U) &= ~0xC00U; // GPIOA MODER register (clear bits)
    
    // 00 - Input (reset state), 01 - General purpose output mode,
    // 10 - Alternate function mode, 11 - Analog mode
    // write '01' to the bit field for pin 5 (bits 10 and 11) to set it as output
    *((unsigned int *)0x50000000U) |=  0x400U; // GPIOA MODER register (set bits)
    
    // 0 - Output push-pull (reset state), 1 - Output open-drain
    // set the output type to push-pull (clear bit 5 in OTYPER register)
    *((unsigned int *)0x50000004U) &= ~0x020U; // GPIOA OTYPER register

    // 00 - Very low speed, 01 - Low speed, 10 - High speed, 11 - Very high speed
    // set the output speed to low speed (clear bits 10 and 11 in OSPEEDR register)
    *((unsigned int *)0x50000008U) &= ~0xC00U; // GPIOA OSPEEDR register
    *((unsigned int *)0x50000008U) |=  0x400U; // GPIOA OSPEEDR register

    // 00 - No pull-up, no pull-down (reset state), 01 - Pull-up, 10 - Pull-down, 11 - Reserved
    // set the pull-up/pull-down configuration to no pull-up, no pull-down (clear bits 10 and 11 in PUPDR register)
    *((unsigned int *)0x5000000CU) &= ~0xC00U; // GPIOA PUPDR register


    while (1) { // endless loop
        // To turn on the LED, we need to set the corresponding bit in the GPIOA BSRR register.
        *((unsigned int *)0x50000018U) = 0x20U; // GPIOA BSRR register

        int volatile counter = 0;
        while (counter < 500000) {  // delay loop
            ++counter;
        }

        // To turn off the LED, we need to reset the corresponding bit in the GPIOA BSRR register.
        *((unsigned int *)0x50000018U) = 0x200000U; // GPIOA BSRR register
        counter = 0;
        while (counter < 500000) {  // delay loop
            ++counter;
        }

    }
    //return 0; // unreachable code
}
