#ifndef UART_H
#define UART_H

#include <stdint.h>

typedef struct {
    uint32_t baudrate;
    uint8_t  tx_pin;
    uint8_t  rx_pin;
} uart_config_t;

void uart_init(const uart_config_t *cfg);
void uart_write_char(char c);
void uart_write_string(const char *buf, uint32_t len);
char uart_read_char(void);
void uart_read_string(char *buf, uint32_t buf_len);

#endif
