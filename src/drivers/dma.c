#include "dma.h"
#include "config/board_config.h"
#include "stm32f446xx.h"

volatile char dma_rx_buf[DMA_RX_BUF_LEN];

void dma1_stream5_init(dma_stream_config_t *cfg) {
  // Enable the DMA1 clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  // Disable stream 5 (USART2_RX) before config
  DMA1_Stream5->CR &= ~DMA_SxCR_EN;
  while (DMA1_Stream5->CR & DMA_SxCR_EN)
    ;

  // Clear interrupt flags for stream 5
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 |
                DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;

  // Start from known state
  DMA1_Stream5->CR = 0;

  // Channel 4 (USART2_RX), memory increment, circular mode, medium priority
  DMA1_Stream5->CR |= (4U << DMA_SxCR_CHSEL_Pos) | DMA_SxCR_MINC |
                      DMA_SxCR_PL_1 | DMA_SxCR_CIRC;

  // 8-bit peripheral & memory, peripheral-to-memory (DIR = 0)
  DMA1_Stream5->CR &= ~(DMA_SxCR_MSIZE_Msk | DMA_SxCR_PSIZE_Msk);

  // Configure addresses
  DMA1_Stream5->PAR =
      (uint32_t)cfg->peripheral_addr;              // Source: UART data register
  DMA1_Stream5->M0AR = (uint32_t)cfg->memory_addr; // Destination: memory buffer
  DMA1_Stream5->NDTR = DMA_RX_BUF_LEN;

  // Enable DMA reception on USART2
  USART2->CR3 |= USART_CR3_DMAR;

  // Enable DMA stream
  DMA1_Stream5->CR |= DMA_SxCR_EN;
}

uint32_t dma1_stream5_get_remaining(void) {
  // NDTR = number of *remaining* transfers in this circular buffer
  return DMA1_Stream5->NDTR;
}
