#include "config/board_config.h"
#include "drivers/rcc.h"
#include "stm32f446xx.h"

#define USE_DMA 1

#if USE_DMA
#include "hal/uart_dma.h"
#else
#include "drivers/uart.h"
#endif

int main(void) {
  rcc_enable_gpioa();

  // Configure LED
  GPIOA->MODER |= (1U << (LED_PIN * 2));
  GPIOA->MODER &= ~(1U << (LED_PIN * 2 + 1));

#if USE_DMA
  uart_dma_init();
  while (1) {
    uart_dma_poll_and_echo();
  }
#else
  uart_config_t cfg = {
      .baudrate = UART_BAUDRATE, .tx_pin = UART_TX_PIN, .rx_pin = UART_RX_PIN};
  uart_init(&cfg);

  while (1) {
    char c = uart_read_char();
    if (c == '\r' || c == '\n') {
      uart_write_string("\r\n", 2);
    } else {
      uart_write_char(c);
    }
    /* uart_write_string("Hello world\r\n", 13); */
  }
#endif
}
