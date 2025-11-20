/* The linker script defines how memory will be organized and how data sections
   will be allocated in the system memory space during program compilation */

ENTRY(Reset_Handler) /* Specifies the program start address */

MEMORY /* define memory regions */
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS /* assign different sections of the program to specific locations in memory */
{
  .isr_vector : /* Contains interrupt vector table */
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text : /* Store executable code */
  {
    . = ALIGN(4);      /* Make sure aligned on 4-byte boundaries */
    *(.text*)
    *(.rodata*)

    . = ALIGN(4);      /* (.) is location counter */
    _etext = .;
  } > FLASH

  /* Address in FLASH where initialised data is stored */
  _sidata = LOADADDR(.data);

  .data : /* Holds initialized variables */
  {
    . = ALIGN(4);
    _sdata = .;

    *(.data*)

    . = ALIGN(4);
    _edata = .;
  } > SRAM AT > FLASH

  .bss : /* Uninitialized variables (zeroed at startup) */
  {
    . = ALIGN(4);
    _sbss = .;

    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;
  } > SRAM
}
