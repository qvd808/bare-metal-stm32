#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

// Clock configuration
#define SYSTEM_CLOCK_HZ     16000000U  // or whatever your actual clock is

// UART configuration
#define UART_BAUDRATE       115200U
#define UART_TX_PIN         2   // PA2
#define UART_RX_PIN         3   // PA3
#define UART_AF             7   // AF7 for USART2

// DMA configuration
#define DMA_RX_BUF_LEN      256

// LED configuration
#define LED_PORT            GPIOA
#define LED_PIN             5

#endif
