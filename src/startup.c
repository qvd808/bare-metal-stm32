/*
 * TODO
 * Configure stack pointer and interupt vector
 * Copy data from Flash to SRAM
 * Init un-init vars with zero
 * Call the main function
 * */

// Usually SP(stack pointer) points to the end of SRAM. Because Cortex-M4
// operations processor rely on a full descending stack, init value of SP should
// be set to the first mem after the topof the stack region

#include "stdint.h"
#include <stdint.h>

#define SRAM_START (0x20000000)
#define SRAM_SIZE (128U * 1024U)
#define SRAM_END ((SRAM_START) + (SRAM_SIZE))

// Init the vector table in the order specifies by the datasheet

void Reset_Handler(void);
void Default_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t isr_vector[] __attribute__((section(".isr_vector"))) = {
    SRAM_END,
    (uint32_t)&Reset_Handler,
    (uint32_t)&NMI_Handler,
    // continue adding device interrupt handlers
};

extern uint32_t _etext, _sdata, _edata, _sbss, _ebss, _sidata;
void main(void);

void Reset_Handler(void) {
  // Copy .data from FLASH to RAM
  uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
  uint8_t *flash_data = (uint8_t *)&_sidata; // Data load address (in flash)
  uint8_t *sram_data = (uint8_t *)&_sdata;   // Data load address (in flash)
  for (uint32_t i = 0; i < data_size; i++) {
    sram_data[i] = flash_data[i];
  }

  // Zero-fill .bss section in SRAM
  uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
  uint8_t *bss = (uint8_t *)&_sbss;

  for (uint32_t i = 0; i < bss_size; i++) {
    bss[i] = 0;
  }

  main();
}

void Default_Handler(void) {
  while (1) {
  }
}
