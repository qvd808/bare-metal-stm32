#ifndef RCC_H
#define RCC_H

#include "stm32f446xx.h"

void rcc_enable_gpioa(void);
void rcc_enable_usart2(void);
void rcc_enable_dma1(void);

uint32_t rcc_get_apb1_clock(void);

#endif
