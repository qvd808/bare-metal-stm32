#include "hal/uart_dma.h"
#include "config/board_config.h"
#include "drivers/dma.h"
#include "drivers/uart.h"
#include "stm32f446xx.h"

static volatile char rx_buffer[DMA_RX_BUF_LEN];
static uint32_t last_read_pos = 0;

void uart_dma_init(void) {
  uart_config_t uart_cfg = {
      .baudrate = UART_BAUDRATE, .tx_pin = UART_TX_PIN, .rx_pin = UART_RX_PIN};
  uart_init(&uart_cfg);

  dma_stream_config_t dma_cfg = {.peripheral_addr =
                                     (volatile uint32_t *)&USART2->DR,
                                 .memory_addr = (uint32_t *)rx_buffer,
                                 .buffer_len = DMA_RX_BUF_LEN,
                                 .channel = 4,
                                 .circular = 1};
  dma1_stream5_init(&dma_cfg);

  uart_enable_dma_rx();

  last_read_pos = DMA_RX_BUF_LEN - dma1_stream5_get_remaining(); // usually 0
}

void uart_dma_poll_and_echo(void) {
  uint32_t write_pos = DMA_RX_BUF_LEN - dma1_stream5_get_remaining();

  while (last_read_pos != write_pos) {
    uart_write_char(rx_buffer[last_read_pos]);
    last_read_pos = (last_read_pos + 1) % DMA_RX_BUF_LEN;
  }
}

int uart_dma_read_line(char *buf, uint32_t buf_len) {
  uint32_t write_pos = DMA_RX_BUF_LEN - dma1_stream5_get_remaining();
  uint32_t idx = 0;

  while (last_read_pos != write_pos) {
    char c = rx_buffer[last_read_pos];
    last_read_pos = (last_read_pos + 1) % DMA_RX_BUF_LEN;

    if (c == '\n' || c == '\r') {
      buf[idx] = '\0';
      return idx; // Return length of line
    }

    if (idx < buf_len - 1) {
      buf[idx++] = c;
    }
  }
  return -1; // No complete line yet
}
