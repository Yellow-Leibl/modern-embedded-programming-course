#include "stm32c031xx.h"  // CMSIS-compatible interface
#include "delay.h"

// LED marked "LD4" on the NUCLEO-C031C6 board
#define LD4_PIN  5U

// external LED to be inserted between GND (short leg) and
// D12 (longer leg) on the CN9 connector
#define LD5_PIN  6U

uint8_t  u8a, u8b;
uint16_t u16c, u16d;
uint32_t u32e, u32f;
uint64_t u64g, u64h;

uint64_t buggy_sum, correct_sum;
int32_t buggy_diff, correct_diff;

int8_t  s8;
int16_t s16;
int32_t s32;

int main(void) {

    u8a  = sizeof(u8a);
    u16c = sizeof(uint16_t);
    u32e = sizeof(uint32_t);
    u64g = sizeof(uint64_t);

    u8a  = 0xa1U;
    u16c = 0xc1c2U;

    u8b  = u8a;
    u16d = u16c;
    u32f = u32e;

    u64h = 3000000000U;   // 3 billion
    u64g = 2000000000U;   // 2 billion
    buggy_sum   = u64h + u64g;            // uint64-math → no overflow!
    correct_sum = (uint64_t)u64h + u64g;  // cast to uint64_t before addition → correct result!

    u32e = 100U;
    buggy_diff   = 10 - u32e;             // int32-math → overflow, because u32e is promoted to int32_t, which is signed, and 100 is greater than INT32_MAX (2,147,483,647)
    correct_diff = 10 - (int32_t)u32e;    // cast to int32_t before subtraction → correct result!

    u32e = 0xffffffffU;  // 4294967295 in decimal, which is greater than INT32_MAX (2,147,483,647)
    if (u32e > -1) {
        u8a = 1U;
    }
    else {
        u8a = 0U;
    }

    u8a = 0xffU;
    if ((uint8_t)(~u8a) == 0x00U) { // true, because ~0xffU is 0x00U
        u8b = 1U;
    }

    u8b = 0xffU;
    if (~u8b == 0x00U) { // ALWAYS false, because ~0xffU is 0x00U, but the expression is evaluated in int, so ~0xffU is 0xffffff00 in 32-bit int representation
        u8b = 2U;
    }
    else {
        u8b = 3U;
    }

    // enable GPIOA clock port for the LEDs
    RCC->IOPENR |= (1U << 0U);

    // NUCLEO-C031C6 board has LED LD4 on GPIOA pin LD4_PIN
    // and external LED LD5 on GPIO LD5_PIN
    // set the LED pins as push-pull output, no pull-up, pull-down
    GPIOA->MODER   &= ~((3U << 2U*LD4_PIN) | (3U << 2U*LD5_PIN));
    GPIOA->MODER   |=  ((1U << 2U*LD4_PIN) | (1U << 2U*LD5_PIN));
    GPIOA->OTYPER  &= ~((1U <<    LD4_PIN) | (1U <<    LD5_PIN));
    GPIOA->OSPEEDR &= ~((3U << 2U*LD4_PIN) | (3U << 2U*LD5_PIN));
    GPIOA->OSPEEDR |=  ((1U << 2U*LD4_PIN) | (1U << 2U*LD5_PIN));
    GPIOA->PUPDR   &= ~((3U << 2U*LD4_PIN) | (3U << 2U*LD5_PIN));

    GPIOA->BSRR = (1U << LD5_PIN); // turn LD5 on

    while (1) { // endless loop
        GPIOA->BSRR = (1U << LD4_PIN); // turn LD4 on

        delay(500000);

        GPIOA->BSRR = (1U << (LD4_PIN + 16U)); // turn LD4 off

        delay(250000);
    }
    //return 0; // unreachable code
}
