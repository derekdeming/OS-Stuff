// start.c
#include "kernel.h"

/**
 * _start - The first C-visible function the CPU jumps to (per linker.ld).
 *          From here, we call kernel_main, then loop forever.
 */
__attribute__((section(".text._start"), noreturn)) void _start(void)
{
    // Optionally set up stack pointer here (if not set by QEMU/OpenSBI).
    // For many simple setups, OpenSBI already sets sp before jumping here.

    kernel_main();

    // If kernel_main ever returns, just spin.
    while (1) {
        __asm__ __volatile__("wfi");
    }
}