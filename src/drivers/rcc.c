#include "stm32f446xx.h"

void rcc_enable_gpioa(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  volatile uint32_t tmp = RCC->AHB1ENR; // Errata 2.2.7
  (void)tmp;
}

void rcc_enable_usart2(void) { RCC->APB1ENR |= RCC_APB1ENR_USART2EN; }

void rcc_enable_dma1(void) { RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; }

uint32_t rcc_get_apb1_clock(void) {
  uint32_t apb1_presc = (RCC->CFGR >> RCC_CFGR_PPRE1_Pos) & 0x7;
  uint32_t apb1_clock = SystemCoreClock; // apb1_clock is initially the system
                                         // clock before prescaler

  if (apb1_presc >= 4) {
    apb1_clock /= (1U << (apb1_presc - 3));
  }

  return apb1_clock;
}
