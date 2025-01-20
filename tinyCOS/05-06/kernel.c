// kernel.c
#include "kernel.h"

// Optional externs if you're clearing BSS or referencing linker symbols:
// extern char __bss[], __bss_end[];

struct sbiret sbi_call(long arg0, long arg1, long arg2,
                       long arg3, long arg4, long arg5,
                       long fid, long eid)
{
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__ ("ecall"
        : "=r"(a0), "=r"(a1)
        : "r"(a0), "r"(a1), "r"(a2),
          "r"(a3), "r"(a4), "r"(a5),
          "r"(a6), "r"(a7)
        : "memory");

    struct sbiret ret;
    ret.error = a0;
    ret.value = a1;
    return ret;
}

void putchar(char ch)
{
    // Using fid=1, eid=0 for a legacy console putchar in many SBI specs.
    sbi_call(ch, 0, 0, 0, 0, 0, 1, 0);
}

void kernel_main(void)
{
    // If using global variables in .bss, zero it out (optional):
    // for (char *b = __bss; b < __bss_end; b++) {
    //     *b = 0;
    // }

    const char *hello = "Hello, World!\n";
    for (const char *p = hello; *p != '\0'; p++) {
        putchar(*p);
    }

    // Halt/idle forever
    while (1) {
        __asm__ __volatile__("wfi");
    }
}

// sbi_call is a function that makes an SBI call to the RISC-V platform.
// It takes a function ID (fid) and an extension ID (eid) as arguments (a6 being the fid and a7 being the eid)
// It returns a struct sbiret containing the error and value of the SBI call.
// we need to make sure all registers except a0 and a1 are preserved because a0 and a1 are used for the returning an error and a1 returning the return values of the SBI call. Ultimately, we need to make sure that the memory is not modified.
// the __asm__ __volatile__ is used to tell the compiler that the assembly code is volatile and that the compiler should not optimize it.