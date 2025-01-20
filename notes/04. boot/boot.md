---
title: Boot
---

# Booting a Simple Kernel with QEMU

## What is "booting"?

When you power on a computer, the CPU doesn't immediately run your operating system. Instead, it starts by running firmware that initializes hardware and then loads the OS code from a storage device. On most PCs, this firmware is BIOS or UEFI. In a QEMU RISC-V virt machine, the equivalent firmware is OpenSBI.

## Role of OpenSBI

The Supervisor Binary Interface (SBI) is a specification that defines what services low-level firmware must provide to an OS kernel—things like writing to a debug console, controlling timers, and powering off the system. OpenSBI is a common implementation of this specification.

When QEMU starts a RISC-V virtual machine (VM) with -bios default, it loads and runs OpenSBI. OpenSBI then sets up the hardware environment (e.g., CPU registers, interrupts, timers) and jumps to the kernel's entry point in memory.

## Running OpenSBI with QEMU

To see this in action, create a script called run.sh:

```bash
#!/bin/bash
set -xue

# QEMU file path (adjust if needed for your system)
QEMU=qemu-system-riscv32

# Start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot
```

Run the script with `./run.sh`. You should see the QEMU console output and OpenSBI's debug messages.

Make it executable:

```bash
chmod +x run.sh
```

Then run:

```bash
./run.sh
```

You should see the OpenSBI banner showing various platform details (e.g., the platform name, timer device, and so on). At this point, QEMU is running but there is no kernel to take over after OpenSBI—so OpenSBI just waits.

### What Happens in the Boot Process?

1. OpenSBI loads and initializes the hardware environment.
2. OpenSBI jumps to the kernel's entry point.
3. The kernel starts executing from the entry point.

This is a very basic boot process. In a real OS, the kernel would initialize more hardware and set up the memory management unit (MMU), page tables, and other components.

### Switching to the QEMU monitor

- Press Ctrl+A then C to switch into the QEMU monitor (a debug console for QEMU).
- Type q to quit QEMU, or use C-a x to exit immediately.

## Creating a Linker Script

A linker script describes where different parts of your kernel (code, read-only data, read/write data, etc.) are placed in memory. Create a file called `kernel.ld`:

```ld
ENTRY(_start)

SECTIONS
{
    . = 0x80000000;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    ...
}
```

This script sets the entry point to `_start` and places the kernel's code, read-only data, read/write data, and bss sections starting at address `0x80000000`. In our case, we reserve 128 KB for the kernel stack after .bss, marking the symbol `__stack_top` at the end of that space.

## Writing a Minimal Kernel

Create a file called `kernel.c`: 

```c
typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;
typedef uint32_t       size_t;

extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *)buf;
    while (n--) {
        *p++ = c;
    }
    return buf;
}

void kernel_main(void) {
    // Clear the BSS section to zero
    memset(__bss, 0, (size_t)(__bss_end - __bss));
    
    // Kernel does nothing yet—just loop forever
    for (;;) {
        // do nothing
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Initialize stack pointer
        "j kernel_main\n"       // Jump to our C kernel
        :
        : [stack_top] "r"(__stack_top)
    );
}
```

### How it works

- `boot()`: This function is placed at address 0x80200000 (because of our linker script) and sets the stack pointer to __stack_top. Then it jumps to kernel_main().
- `kernel_main()`: We first zero out .bss manually (some bootloaders do this automatically, but here we do it ourselves). Then we loop forever so we can see that the kernel is loaded and running.
- `__attribute__((naked))`: Tells the compiler not to insert extra instructions (like setting up a new stack frame) in boot().
- Symbols: `__bss, __bss_end, and __stack_top` come from the linker script.

## Building and Running the Kernel

Update the run.sh script to compile your kernel and add the `-kernel` argument to QEMU:

```bash
#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

# path to clang and compiler flags
CC=/opt/homebrew/opt/llvm/bin/clang  # you will need to change this if you use a different setup
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# build the kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf kernel.c

# start QEMU
$QEMU -machine virt \
      -bios default \
      -nographic \
      -serial mon:stdio \
      --no-reboot \
      -kernel kernel.elf
```

Run `./run.sh` again. You’ll see OpenSBI’s banner, and then the system will jump to our kernel. Because the kernel loops forever with no output, it seems like nothing is happening—but that’s expected.

## Debugging the Kernel

To debug the kernel, you can use the QEMU monitor. Press Ctrl+A then C to switch into the QEMU monitor, then type `info registers` to see the current state of the CPU registers.

You should see:
- `pc` (Program Counter) near 0x80200010 or 0x80200014, which corresponds to the jump instruction in kernel_main.
- `sp` (Stack Pointer) set to __stack_top (e.g., 0x80220018 if you used 128KB for the stack).

You can also use the `-d` option to QEMU to enable debugging:

```bash
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -d int -D qemu.log
```

This will create a file called `qemu.log` with debugging information. If you disassemble kernel.elf using `llvm-objdump -d kernel.elf`, you can match these addresses to the actual instructions in the code. You can also inspect kernel.map to see how the linker assigned addresses.

**Congratulations!** We just built and ran a bare-bones kernel on QEMU with OpenSBI handling the low-level initialization. Although our kernel doesn’t print anything, it is running. From here, you can expand this setup to initialize hardware, implement drivers, and build more complex operating system features.

---

## Key Takeaways for beginners:
	1.	Firmware (OpenSBI): Initializes hardware on RISC-V before jumping to the kernel.
	2.	Linker Script: Tells the linker how to arrange your kernel’s memory layout.
	3.	Boot Code (boot()): Must be placed at the load address (here 0x80200000) so that firmware can jump to it.
	4.	Stack Setup: The kernel must set its own stack pointer because there is no standard C runtime environment at this stage.
	5.	Infinite Loop: With no output or driver code, the kernel appears idle, but it’s actually running code that does nothing.

That’s the entire flow—power on (or run.sh in QEMU) → OpenSBI initializes → your kernel code takes over. It’s minimal, but all subsequent kernel development builds on these exact basics.