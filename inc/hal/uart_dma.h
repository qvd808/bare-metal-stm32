#ifndef UART_DMA_H
#define UART_DMA_H

#include <stdint.h>

void uart_dma_init(void);
void uart_dma_poll_and_echo(void);
int  uart_dma_read_line(char *buf, uint32_t buf_len);

#endif
