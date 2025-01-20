---
title: Hello World
---

# Hello World in RISC-V

In this section, we'll walk through the process of printing "Hello, World!" from our fledgling RISC-V kernel. This typically involves invoking the [Supervisor Binary Interface (SBI)](https://github.com/riscv-non-isa/riscv-sbi-doc) provided by the OpenSBI firmware. By understanding SBI calls, we can better understand how low-level firmware services can be used by your kernel.

---

## 1. What is SBI?

RISC-V is designed with multiple privilege levels (M, S, U). Your kernel runs in **S-Mode** (Supervisor Mode), while OpenSBI runs in **M-Mode** (Machine Mode), which is the most privileged. Because your kernel cannot directly interact with hardware registers from S-Mode in a standard-compliant environment, you need a way to request operations (like printing characters to a console). That’s where SBI comes in:

- **Supervisor Binary Interface (SBI)**: A standard way (API) for S-Mode software (i.e., your kernel) to request services from M-Mode firmware (i.e., OpenSBI).

Using an SBI call is akin to making a function call into firmware. Under the hood, it uses the `ecall` instruction to trap from S-Mode into M-Mode.

---

## Our Project Structure

```
├── kernel.c (main kernel code / entry point in S-mode)
├── kernel.h (defs shared by kernel code)
├── run.sh (script to compile and run the kernel on QEMU)
├── linker.ld (optional script to place code, data, and stack in memory)
├── ../05.helloWorld/helloworld.md (notes)
```

## Making SBI Calls

To make an SBI call, we need to:
0. Define the function struct in `kernel.h` (this is the struct that will be used to return the error and value of the SBI call)
1. Put arguments in registers a0-a7
2. Execute `ecall`
3. Get return values from a0-a1

First we have our kernel header file `kernel.h` which contains the definition of the `sbiret` struct.

```c
#ifndef KERNEL_H
#define KERNEL_H

struct sbiret {
    long error;
    long value;
};
#endif
```

Next we have our kernel source file `kernel.c` which contains the main kernel code.

```c
// kernel.c
#include "kernel.h"

// Symbols from the linker script (if you are clearing .bss, etc.)
// extern char __bss[], __bss_end[], __stack_top[];

/**
 * sbi_call - A function to invoke an SBI call
 * @arg0-arg5: up to six possible arguments for the call
 * @fid: Function ID (varies depending on the specific SBI call)
 * @eid: Extension ID (identifies which SBI extension to use)
 *
 * Returns a struct sbiret containing:
 *  - error: error code (0 if success, or a negative value if error)
 *  - value: additional return value on success
 */
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
        : "r"(a0), "r"(a1), "r"(a2), "r"(a3),
          "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory");

    // The SBI spec indicates a0 and a1 are used for return values:
    // a0 => error, a1 => value
    struct sbiret ret;
    ret.error = a0;
    ret.value = a1;
    return ret;
}
```

We are first placing arguments in registers a0-a5 (the first 6 arguments to the SBI call). We then place the function ID (fid) in register a6 and the extension ID (eid) in register a7.

Next we execute the `ecall` instruction. The processor jumps to M-Mode to the OpenSBI handler.

Finally, we return the error (a0) and value (a1) of the SBI call in the `sbiret` struct and return to S-Mode.

---

As part of our kernel code, we print a character (PutChar) to the console. SBI provides a “legacy” console output function known as Console Putchar. The function ID and extension ID for console putchar can vary slightly based on SBI version. In many references, you’ll see:

- Extension ID (eid): 0x1
- Function ID (fid): Usually 0, or the reversed approach for older 0.1 specs can appear.

We can use an example of extension ID 0 with function ID 1.

```c
// (arg0=ch, arg1-arg5=0, fid=1, eid=0)
// This effectively calls the "Console Putchar" function in OpenSBI.
void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 1, 0);
}
```

Whenever you call `putchar('A')`, one character is forwarded to OpenSBI, which then writes it out to the QEMU UART (serial port) and eventually you see `'A'` on your screen.

---

## 2. Hello World Kernel

Now let's tie things together with `kernel_main()`. One option we have is to clear out the `.bss` section (if using C variables that need to be initialized to 0), print our `Hello, World!` message, and then enter an infinite loop (often idle + wfi).

```c
// kernel.c (continued from above)

void kernel_main(void)
{
    // Optionally clear .bss if needed:
    // for (char *bss = __bss; bss < __bss_end; bss++) {
    //     *bss = 0;
    // }

    const char *hello = "Hello, World!\n";
    for (const char *p = hello; *p != '\0'; p++) {
        putchar(*p);
    }

    // Enter an infinite loop, often halting with 'wfi' (wait-for-interrupt).
    // This conserves power or cycles until an interrupt or event arrives.
    while (1) {
        __asm__ __volatile__("wfi");
    }
}
```

---

## 3. Compiling and Running the Kernel

We can write a simple linker script to place our code, data, and stack in memory. It may look something like this:

```ld
// linker.ld
OUTPUT_ARCH(riscv)
ENTRY(_start);

SECTIONS
{
    . = 0x80000000;          /* Kernel load address in QEMU for S-Mode */
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) *(COMMON) }
    . = ALIGN(8);
    _end = .;
}
```

where 0x80000000 is the load address of the kernel in QEMU for S-Mode.

To compile and run the kernel, we can use the `run.sh` script.

```bash
./run.sh
```
