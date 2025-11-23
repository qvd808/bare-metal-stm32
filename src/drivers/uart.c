#include "drivers/uart.h"
#include "drivers/rcc.h"
#include "stm32f446xx.h"

#define USE_OVER8 0        // set to 1 if you want oversampling by 8
#define GPIO_MODE_AF 0x2U  // 10b
#define GPIO_AF7_USART2 7U // AF7

void uart_init(const uart_config_t *cfg) {
  (void)cfg; // cfg not used if we hardcode pins

  rcc_enable_gpioa();
  rcc_enable_usart2();

  /* 1. Configure PA2, PA3 as AF */
  GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
  GPIOA->MODER |= (GPIO_MODE_AF << GPIO_MODER_MODE2_Pos) |
                  (GPIO_MODE_AF << GPIO_MODER_MODE3_Pos);

  /* 2. Select AF7 (USART2) for PA2, PA3 */
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
  GPIOA->AFR[0] |= (GPIO_AF7_USART2 << GPIO_AFRL_AFSEL2_Pos) |
                   (GPIO_AF7_USART2 << GPIO_AFRL_AFSEL3_Pos);

  /* 3. Configure baud rate */
  uint32_t apb1_clk = rcc_get_apb1_clock();

#if USE_OVER8
  USART2->CR1 |= USART_CR1_OVER8;
  uint32_t usartdiv = (2 * apb1_clk) / cfg->baudrate;
  uint32_t mantissa = usartdiv >> 4;
  uint32_t fraction = (usartdiv & 0xF) >> 1; // 3 bits used for OVER8
  USART2->BRR = (mantissa << 4) | fraction;
#else
  USART2->BRR = apb1_clk / cfg->baudrate;
#endif

  /* 4. Enable USART2 (TX + RX) */
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

  return (char)USART2->DR;
}

void uart_read_string(char *buf, uint32_t buf_len) {
  for (uint32_t i = 0; i < buf_len - 1; i++) {
    buf[i] = uart_read_char();
  }
  buf[buf_len - 1] = '\0';
}
