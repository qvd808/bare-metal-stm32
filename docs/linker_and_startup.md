- The linker and start up is pull from the https://github.com/STMicroelectronics/cmsis-device-f4.git
- The linker script defines how memory will be organized and how data sections will be allocated in the system memory space during program compilation such as
    - ENTRY: Specifies the program start address, with the functions that will be called in start up before calling main
    - Memory: Defines the memory regions, usually FLASH and SRAM
    - SECTIONS
        - .isr_vector: Contains the interrupt vector tables
        - .text: Store exectable code
        - .data: Holds intialized variables
        - .bss: Holds Uninitialized variable (will be zeroed at the start up code)
- Usually .data and .bss is align in 4 bytes
- CMSIS start up files also required _estack which points to the top of the stack (ends of SRAM)

### Weird things recorded for educations purposes?
- So apparently when linking there's a function __libc_init_array function from the start up which take care of initialized C++ contrucstor ![this link](https://stackoverflow.com/questions/13734745/why-do-i-have-an-undefined-reference-to-init-in-libc-init-array). Therefore, if we are going to use libc, we have to do sth about this
    - For now, just add a stub libc calls somewhere so the compiler is happy
