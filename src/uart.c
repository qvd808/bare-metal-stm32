#include "uart.h"
#include "stm32f446xx.h"

const uint32_t baudrate = 115200;

void uart_init(void) {
  /* 1. Enable GPIOA clock (if not already done elsewhere) */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* 2. Enable USART2 clock */
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  /* 3. Configure PA2 and PA3 as Alternate Function */
  GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
  GPIOA->MODER |= (2U << GPIO_MODER_MODE2_Pos) |
                  (2U << GPIO_MODER_MODE3_Pos); // AF mode = 10b

  /* 4. Select AF7 for PA2, PA3 */
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
  GPIOA->AFR[0] |= (7U << GPIO_AFRL_AFSEL2_Pos) | (7U << GPIO_AFRL_AFSEL3_Pos);

  /* 5. Compute correct PCLK1 frequency for USART2 */
  uint32_t apb1_presc = (RCC->CFGR >> RCC_CFGR_PPRE1_Pos) & 0x7;
  uint32_t apb1_clock = SystemCoreClock; // apb1_clock is initially the system
                                         // clock before prescaler

  if (apb1_presc >= 4) {
    apb1_clock /= (1U << (apb1_presc - 3));
  }

  /*
   * USARTDIV = fCK / (16 * baud) (over sampling by 16)
   *          = fCK / (8 * baud) (over sampling by 8)
   * div16  = 16 * USARTDIV
   * in case of over sampling by 16 ~= fCK / baud. We can do more to get better
   * resolution
   * Same thing with over sampling by 8
   * Ref:
   * https://electronics.stackexchange.com/questions/502135/understanding-calculations-for-baud-rate-fractional-generator-stm32f4
   */

#define USE_OVER8 1 // set to 1 if you want oversampling by 8

#if USE_OVER8
  USART2->CR1 |= USART_CR1_OVER8;
  uint32_t usartdiv =
      (2 * apb1_clock) / baudrate; // fixed-point with 4 fractional bits
  uint32_t mantissa = usartdiv >> 4;
  uint32_t fraction = (usartdiv & 0xF) >> 1; // only 3 bits used for OVER8
  USART2->BRR = (mantissa << 4) | fraction;
#else
  // Oversampling by 16 (default)
  USART2->BRR = apb1_clock / baudrate;
#endif

  /* 7. Enable USART2: 8N1, Rx/Tx */
  USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart_write_char(char c) {
  while (!(USART2->SR & USART_SR_TXE))
    ;
  USART2->DR = (uint32_t)c;
}

void uart_write_string(const char *buf, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    uart_write_char(buf[i]);
  }
}

char uart_read_char(void) {
  while (!(USART2->SR & USART_SR_RXNE))
    ;
  char c = USART2->DR;
  return c;
}

void read_string(char *buf, uint32_t buf_len) {
  int i = 0;
  while (i < buf_len - 1) {
    buf[i] = uart_read_char();
    i++;
  }
  buf[i] = '\0';
}
