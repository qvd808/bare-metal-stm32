#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>

#define LED_PIN 5

void main(void) {
  // Enable clock for GPIOA peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // Device erreta 2.2.7 - Delay after an RCC pheripheral clock
  volatile uint32_t tmp = RCC->AHB1ENR;

  // Configure the PA5 pin as output (LED on the Nucleo board)
  GPIOA->MODER |= (1 << 10);  // MODER5[1:0] = 01 (Output)
  GPIOA->MODER &= ~(1 << 11); // Ensure that MODER5[1] is cleared

  uart_init();

  while (1) {
    // Toggle the LED
    GPIOA->ODR ^= (1 << LED_PIN);
    for (uint32_t i = 0; i < 5000000; i++)
      ; // Simple delay
    char c = uart_read_char();
    uart_write_char(c);
  }
}
