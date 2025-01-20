#!/bin/bash
set -xue # -xue is used to print the commands and their arguments as they are executed.

# QEMU file path (for 32-bit RISC-V)
QEMU=qemu-system-riscv32

# Path to Clang and compiler flags
CC=/opt/homebrew/opt/llvm/bin/clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# Build the kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf start.c kernel.c

# Launch QEMU
#  -machine virt    : Use the 'virt' board model for RISC-V
#  -m 128M          : Provide 128 MB of RAM (adjust as you wish)
#  -bios default    : Use OpenSBI included in QEMU
#  -nographic       : Disable graphics, use stdio for I/O
#  -serial mon:stdio: Combine QEMU monitor with stdio
#  --no-reboot      : Do not reboot on exit
#  -kernel kernel.elf: Our kernel binary
$QEMU -machine virt -m 128M -bios none -nographic -serial mon:stdio --no-reboot -kernel kernel.elf