#include "drivers/uart.h"
#include "drivers/rcc.h"
#include "stm32f446xx.h"

#define USE_OVER8 0 // set to 1 if you want oversampling by 8

void uart_init(const uart_config_t *cfg) {
  rcc_enable_gpioa();
  rcc_enable_usart2();

  // Configure pins as AF
  uint32_t tx = cfg->tx_pin;
  uint32_t rx = cfg->rx_pin;

  GPIOA->MODER &= ~((3U << (tx * 2)) | (3U << (rx * 2)));
  GPIOA->MODER |= ((2U << (tx * 2)) | (2U << (rx * 2)));

  GPIOA->AFR[0] &= ~((0xFU << (tx * 4)) | (0xFU << (rx * 4)));
  GPIOA->AFR[0] |= ((7U << (tx * 4)) | (7U << (rx * 4)));

  // Configure baud rate
  uint32_t apb1_clk = rcc_get_apb1_clock();

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

#if USE_OVER8
  USART2->CR1 |= USART_CR1_OVER8;
  uint32_t usartdiv =
      (2 * apb1_clock) / baudrate; // fixed-point with 4 fractional bits
  uint32_t mantissa = usartdiv >> 4;
  uint32_t fraction = (usartdiv & 0xF) >> 1; // only 3 bits used for OVER8
  USART2->BRR = (mantissa << 4) | fraction;
#else
  // Oversampling by 16 (default)
  USART2->BRR = apb1_clk / cfg->baudrate;
#endif

  // Enable USART
  USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart_enable_dma_rx(void) { USART2->CR3 |= USART_CR3_DMAR; }

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

  return (char)USART2->DR;
}

void uart_read_string(char *buf, uint32_t buf_len) {
  for (uint32_t i = 0; i < buf_len - 1; i++) {
    buf[i] = uart_read_char();
  }
  buf[buf_len - 1] = '\0';
}
