---
title: RISC-V Basics
---

# RISC-V Basics

Just like web browsers hide the differences between **Windows/macOS/Linux**, operating systems hide the differences between **CPUs**. In other words, an **operating system** is a program that controls the CPU to provide an abstraction layer for applications. When you open a text editor or run a web browser, your OS ensures that all the nuances of the CPU, memory, input devices, and more remain hidden, giving you (the user) a consistent experience.

In this book, I chose **RISC-V** as the target CPU because:

- [The specification](https://riscv.org/technical/specifications/) is simple and suitable for beginners.
- It's a trending **ISA** (Instruction Set Architecture) in recent years, alongside **x86** and **Arm**.
- The design decisions are well-documented throughout the spec and are genuinely fun to read and explore.

We will write an OS for **32-bit** RISC-V. Of course, you could write for **64-bit** RISC-V with only a few changes. However, the wider bit width makes it slightly more complex, and the longer addresses can be tedious to read. Sticking to 32-bit means our address space ranges up to `0xFFFFFFFF` (about 4GB), which is plenty for our educational OS goals.

---

## QEMU virtual machine

Computers are composed of various devices: **CPU**, **memory**, **network cards**, **hard disks**, **GPUs**, **audio cards**, **etc**. Even if two systems share the same CPU architecture, differences in their peripheral devices (like how they handle I/O or manage onboard components) can make them quite distinct. For example, although **iPhone** and **Raspberry Pi** both use **Arm** CPUs, they differ significantly in their hardware setup. 

In this book, we support the **QEMU** `virt` machine ([documentation](https://www.qemu.org/docs/master/system/riscv/virt.html)) because:

- Even though it’s a “virtual” machine and does not exist in physical hardware form, it’s *simple* and very similar to real devices.
- You can emulate it on **QEMU** for free. You don’t need to buy any special hardware or device.
- When you encounter debugging issues, you can read QEMU’s source code or attach a debugger to the QEMU process to pinpoint and fix what’s wrong.

> [!TIP]
> **Installing QEMU**  
> For most Linux distributions, installing QEMU is as easy as:
> ```bash
> sudo apt-get install qemu-system-misc
> ```
> or similar. For macOS, you can use:
> ```bash
> brew install qemu
> ```
> For Windows, you can download pre-built binaries from the official [QEMU website](https://www.qemu.org/download/), or use package managers like [chocolatey](https://chocolatey.org/) or [scoop](https://scoop.sh/).

---

## RISC-V assembly 101

**RISC-V**, or **RISC-V ISA** (Instruction Set Architecture), defines the instructions that the CPU can execute. It’s analogous to an API specification or a programming language specification for low-level operations. When you write a **C** program, the compiler translates your code into RISC-V assembly. Sometimes, as an OS developer, you’ll need to drop down to assembly to control hardware features at a very granular level.

But don’t worry! **Assembly** is not as mysterious or complicated as it might initially seem.

> [!TIP]
> **Try Compiler Explorer!**  
> A useful tool for learning assembly is [Compiler Explorer](https://godbolt.org/), an online compiler. As you type C code, it shows the corresponding assembly code side-by-side.
>
> - By default, Compiler Explorer uses **x86-64** assembly. Specify `RISC-V rv32gc clang (trunk)` in the right pane to output 32-bit RISC-V assembly.
> - It’s also enlightening to use different optimization options like `-O0` (no optimization) or `-O2` (optimizations on) and see how the generated assembly changes.

### Assembly language basics

Assembly language is a mostly direct representation of **machine code**. Let's look at a simple example:

```asm
addi a0, a1, 123
```

Let's break this instruction down:

1. **Instruction name (opcode)**: `addi`
   - This tells the CPU to perform an add immediate operation.

2. **Operands**: `a0`, `a1`, `123`
   - `a0` is the destination register (where the result is stored)
   - `a1` is the source register (the starting value)
   - `123` is the immediate value to add

This single instruction effectively does:

```c
a0 = a1 + 123
```

in "assembly-speak".

### Registers

**Registers** are like very fast temporary variables in the CPU. Typically, the CPU loads data from memory into registers, performs arithmetic or logical operations in registers, and writes the results back to memory or other registers. 

Here are some common 32-bit registers in RISC-V:

| Register | ABI Name (alias) | Description |
|---| -------- | ----------- |
| `pc` | `pc`       | Program counter (where the next instruction is) |
| `x0` |`zero`     | Hardwired zero (always reads as zero) |
| `x1` |`ra`         | Return address |
| `x2` |`sp`         | Stack pointer |
| `x5` - `x7` | `t0` - `t2` | Temporary registers |
| `x8` | `fp`      | Stack frame pointer |
| `x10` - `x11` | `a0` - `a1`  | Function arguments/return values |
| `x12` - `x17` | `a2` - `a7`  | Function arguments |
| `x18` - `x27` | `s0` - `s11` | Temporary registers saved across calls |
| `x28` - `x31` | `t3` - `t6` | More temporary registers |

> [!TIP]
>
> **Calling convention:**
>
> Although you can use CPU registers as you see fit, to ensure interoperability with other software, usage is defined by a *calling convention*.
>
> For example, `x10` - `x11` registers are used for function arguments and return values. 
>
> **Return register**: `a0-a1` (store function return value here)
>
> **Argument registers**: `a2-a7` (store function arguments here)
>
> **Saved registers**: `s0-s11` (need to be preserved if you modify them in a function)
>
> Check [the spec](https://riscv.org/wp-content/uploads/2015/01/riscv-calling.pdf) for more details.

### Memory Access

Since registers are limited in number and size, most data is kept in memory. Programs read and write data from/to memory using load and store instructions. In 32-bit RISC-V, the relevant instructions are often:

```asm
lw rd, imm(rs1)
sw rs2, imm(rs1)

lw a0, 0(a1) # Load word from address in a1 into a0. Conceptually, it's like `a0 = *a1`; in C (if a1 is a pointer), it's like `a0 = *(int*)a1`

sw a0, 0(a1) # Store word from a0 into address in a1. Conceptually, it's like `*a1 = a0`; in C (if a1 is a pointer), it's like `*(int*)a1 = a0`
```

The `0(a1)` part indicates that there is an offset of 0 from the address in `a1`. You can also do something like `4(a1)`, which means an offset of 4 bytes from the address in `a1`. This is akin to pointer arithmetic in C.

- `lw` loads a word (32-bit) from memory into a register.
- `sw` stores a word (32-bit) from a register to memory.
- `imm` is an immediate offset (a constant value)
- `rs1` is the source register (the address to read from or write to)
- `rs2` is the destination register (the value to write)

### Addressing modes

In RISC-V, memory addresses can be specified in several ways:

- **Immediate addressing**: The operand value is directly encoded in the instruction itself
  ```asm
  li a0, 42      # Load immediate value 42 into a0
  ```

- **Register addressing**: The operand is in a register
  ```asm
  mv a0, a1      # Copy value from a1 to a0
  ```

- **Base + offset addressing**: The effective address is computed by adding a signed offset to a base register
  ```asm
  lw a0, 8(sp)   # Load word from memory at address sp + 8
  sw a1, -4(fp)  # Store word to memory at address fp - 4
  ```

This flexibility in addressing modes allows for efficient memory access patterns. Base + offset addressing is particularly useful for:
- Accessing stack variables (using sp as base)
- Accessing struct/array members (using a pointer as base)
- Accessing function parameters and local variables (using fp as base)

### Branching Instructions

Branching instructions allow the CPU to change the flow of execution based on the value of a condition. They let you implement if, for, while, and other control flow statements in higher-level languages. For example:

```asm
beq a0, a1, label  # Branch if a0 equals a1. Equivalent to `if (a0 == a1) { goto label; }`
bne a0, a1, label  # Branch if a0 is not equal to a1. Equivalent to `if (a0 != a1) { goto label; }`
blt a0, a1, label  # Branch if a0 is less than a1. Equivalent to `if (a0 < a1) { goto label; }`
bge a0, a1, label  # Branch if a0 is greater than or equal to a1. Equivalent to `if (a0 >= a1) { goto label; }`
bnez a0, label     # Branch if a0 is not zero. Equivalent to `if (a0 != 0) { goto label; }`
```

### Function Calls

Function calls are a way to call other functions. They allow you to implement modularity and reuse code.

```asm
li  a0, 123       # Load the immediate value 123 into a0
jal ra, my_func   # Jump to my_func, storing the return address in ra (x1)

# Execution continues here after my_func returns...

my_func:
    addi a0, a0, 1  # a0 was 123, now a0 = a0 + 1 = 124
    ret             # Return, with a0 as the return value
```

In this example, **arguments** are placed in `a0-a7` (for up to 8 integer arguments). The `jal` instruction is used to jump to a function and store the return address in `ra`. The `ret` instruction is used to return from a function. Basically, `jal ra, my_func` places the next instruction's address in `ra`, and then jumps to `my_func`. When `my_func` is done, it uses `ret` to jump back to the instruction after `jal ra, my_func`.

### Stack

The stack is a region of memory used for storing temporary data, function arguments, and return values. It's a Last-In-First-Out (LIFO) data structure. The stack is a region of memory that grows downwards (from higher addresses to lower addresses). It’s primarily used for:

- Function calls (storing return addresses, local variables).
- Local variables in languages like C.

#### Pushing onto the stack

To push (store) a register value onto the stack, you do something like:

```asm
sw a0, 0(sp) # Store the value of a0 into the memory address at sp
addi sp, sp, -4 # Decrement sp by 4 to point to the next free slot on the stack
```

#### Popping from the stack

To pop (retrieve) a register value from the stack, you do something like:

```asm
lw a0, 0(sp) # Load the value from the memory address at sp into a0
addi sp, sp, 4 # Increment sp by 4 to point to the next slot on the stack
```

[!TIP]
In C, the compiler automatically generates these stack operations for local variables and function calls, so you rarely need to write them manually in assembly.

---

### CPU Modes

A CPU can operate in multiple privileged levels. RISC-V defines 3 core modes (U-mode, S-mode, M-mode) and 2 optional modes (H-mode, D-mode):

| Mode | Description |
|------|-------------|
| **User mode (U-mode)** | Normal user applications run here with the least privilege. |
| **Supervisor mode (S-mode)** | The operating system (OS) kernel or hypervisor host typically runs here. |
| **Machine mode (M-mode)** | The highest privilege; essentially “bare metal.” Firmware (like OpenSBI) and low-level initialization run in M-mode. |
| **Hypervisor mode (H-mode)** | An *optional* extension in newer RISC-V specs. If implemented, it allows a hypervisor/virtual machine monitor (VMM) to manage multiple virtual machines. |
| **Debug mode (D-mode)** | A *special* mode generally used for hardware or JTAG debugging. It’s often described separately from the standard privilege levels since it’s an out-of-band mechanism used by debuggers. |

---

### Privileged Instructions

In **RISC-V**, some instructions can only be executed by the operating system or low-level firmware (i.e., in **S-mode** or **M-mode**) because they manage critical hardware resources and system state. If a user-mode program attempts to run these instructions, the CPU triggers a **trap** to prevent the application from violating security or stability constraints.

Below are some of the **privileged instructions** we will use in this book. They are essential for tasks like reading and writing **Control and Status Registers (CSRs)**, handling traps, and invalidating cached memory translations.

| Opcode & Operands    | Description                                                                 | Pseudocode                                  |
|----------------------|-----------------------------------------------------------------------------|---------------------------------------------|
| `csrr rd, csr`       | **Read** from a CSR (Control and Status Register)                           | `rd = csr;`                                 |
| `csrw csr, rs`       | **Write** to a CSR                                                          | `csr = rs;`                                 |
| `csrrw rd, csr, rs`  | **Read** from and **write** to a CSR at once                                | `tmp = csr; csr = rs; rd = tmp;`            |
| `sret`               | **Return** from a trap handler, restoring the previous context (PC, mode)   | *(varies internally)*                       |
| `sfence.vma`         | **Invalidate** (flush) the Translation Lookaside Buffer (**TLB**)           | *(varies internally)*                       |

A **CSR (Control and Status Register)** holds various hardware and configuration settings, such as interrupt vectors, trap addresses, or CPU mode information. The complete list of CSRs and their purposes can be found in the [RISC-V Privileged Specification](https://riscv.org/specifications/privileged-isa/).

> [!TIP]
> **Digging Deeper**
> Some instructions, such as `sret`, perform multiple hidden steps (e.g., changing CPU mode and restoring program counters). If you're curious about the low-level details, check out a RISC-V emulator’s source code. For example, [rvemu](https://github.com/d0iasm/rvemu) has concise and readable implementations (see their [sret logic](https://github.com/d0iasm/rvemu/blob/f55eb5b376f22a73c0cf2630848c03f8d5c93922/src/cpu.rs#L3357-L3400)) that illustrate what happens behind the scenes. Understanding these nuances will help you debug more effectively when developing your OS.

---

### Inline Assembly

In the upcoming chapters, you'll often see **inline assembly**, which is a way to write assembly code directly inside your C source files. Although it's possible to place assembly in a separate file (typically with a `.S` extension), inline assembly provides several advantages:

1. **Direct usage of C variables:**
   You can reference C variables in your assembly code and store the results back into those variables without extra boilerplate.
2. **Automatic register management:**
   The compiler knows which registers you plan to use (through constraints) and saves/restores them as needed, reducing the risk of accidentally clobbering important data.
3. **Better optimization:**
   Inline assembly can be optimized by the compiler in conjunction with your C code, resulting in more efficient binaries (especially if using `__volatile__` is not strictly necessary for every case).

### Example of Inline Assembly

```c
uint32_t value;
__asm__ __volatile__("csrr %0, sepc" : "=r"(value));
```

In this example, the `csrr` instruction reads the `sepc` CSR into the `value` variable. The `volatile` keyword ensures that the compiler doesn't optimize away the instruction, which is crucial for correctness in low-level code.

#### Inline Assembly Syntax

The general syntax for inline assembly in GCC/Clang is:

```c
__asm__ __volatile__(
    "assembly code"
    : output operands (example: "=r"(value))
    : input operands (example: "r"(0))
    : clobbered registers (example: "memory")
);

__asm__("assembly code" : "=r"(value) : "r"(0) : "memory");
```

- **assembly code**: A string literal containing assembly instructions to be executed.
- **output operands**: A list of operands that will receive values from the assembly code.
- **input operands**: A list of operands that will provide values to the assembly code.
- **clobbered registers**: A list of registers that are modified by the assembly code. If you omit them, the compiler might resuse these registers, causing incorrect behavior.

| Part | Description |
|------|-------------|
| `__asm__` | Indicates that you're writing inline assembly. |
| `__volatile__` | Prevents the compiler from optimizing or reordering the assembly block. |
| `"assembly code"` | Actual instructions, written as a string literal. |
| `output operands` | Variables in which you store the results of the assembly block. |
| `input operands` | Constants or variables you pass into the assembly block. |
| `clobbered registers` | Registers modified by your assembly. Telling the compiler avoids corrupting those registers' data. |


#### Operand Constraints

Each operand follows a `constraint` letter that informs the compiler how to handle it (e.g., is it stored in a register, does it accept immediate values, etc.). Common constraints include:

- `r`: For an input operand, the operand is a register. For an output operand, the operand is a register that will receive the result.
- `i`: The operand is an immediate value.
- `m`: The operand is a memory address.
- `a`: The operand is a special register (e.g., `a0` for function arguments).

---

### System Calls

In a running system, user mode applications often need services that only the kernel can provide, such as reading from the keyboard, writing to the screen, accessing files, or communicating with peripherals. These requests are made through system calls.

The `ecall` instruction is used to make a system call. The specific system call is determined by the `a7` register.

RISC-V defines the `ecall` instruction (short for environment call) to trigger a trap into the kernel (running in S-mode or M-mode, depending on the setup). Conceptually, the process looks like this:

1. A user-mode program prepares arguments in the appropriate registers (e.g., a0, a1, …, a7), including something like a system call number in a7 (common in many RISC-V OS examples).
2. It executes:
    ```asm
    ecall
    ```
3. The CPU then jumps to a trap handler in the kernel(S-mode).
4. The kernel trap handler checks the cause of the trap (which will indicate an `ecall` from user mode).
5. Based on the system call number (ex: a7 = 4 for print), the kernel performs the appropriate action (e.g., read, write, open file, etc.)
6. The result of the operation is placed back into one of the argument registers (e.g., a0 for return value), and the kernel returns to user mode (using `sret` or a similar mechanism).

For example, a user program might do something like this:

```c
// Pseudocode in C for a minimal system call example
static inline int my_syscall(int syscall_num, int arg) {
    register int a0 asm("a0") = arg;       // First argument
    register int a7 asm("a7") = syscall_num; // Syscall number
    __asm__ __volatile__("ecall"
                         : "+r"(a0)        // a0 is both input (arg) and output (return value)
                         : "r"(a7)
                         : "memory");
    return a0;  // The kernel places the return value in a0
}
```