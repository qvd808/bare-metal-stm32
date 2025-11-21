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
    GPIOA->MODER |=  (2U << GPIO_MODER_MODE2_Pos) |
                     (2U << GPIO_MODER_MODE3_Pos);   // AF mode = 10b

    /* 4. Select AF7 for PA2, PA3 */
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |=  (7U << GPIO_AFRL_AFSEL2_Pos) |
                      (7U << GPIO_AFRL_AFSEL3_Pos);

    /* 5. Compute correct PCLK1 frequency for USART2 */
    uint32_t pclk1 = SystemCoreClock;
    uint32_t presc = (RCC->CFGR >> RCC_CFGR_PPRE1_Pos) & 0x7;

    if (presc >= 4) {
        pclk1 /= (1U << (presc - 3));
    }

    /* 6. Set baud rate */
    USART2->BRR = pclk1 / baudrate;

    /* 7. Enable USART2: 8N1, Rx/Tx */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart_write_char(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = (uint32_t)c;
}
