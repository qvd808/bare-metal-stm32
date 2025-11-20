// Putting it here so the linker happy since the CMSIS have this function
// __libc_init_array which need _init function. For now, just ignore it:
// https://stackoverflow.com/questions/13734745/why-do-i-have-an-undefined-reference-to-init-in-libc-init-array
void _init(void) {}
