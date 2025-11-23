#ifndef DMA_H
#define DMA_H

#include <stdint.h>

typedef struct {
    volatile uint32_t *peripheral_addr;
    uint32_t *memory_addr;
    uint32_t           buffer_len;
    uint8_t            channel;      // 0-7
    uint8_t            circular;     // 0 or 1
} dma_stream_config_t;

void dma1_stream5_init(dma_stream_config_t *cfg);
uint32_t dma1_stream5_get_remaining(void);

#endif
