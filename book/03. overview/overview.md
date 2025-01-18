---
title: Overview
---

# What we will implement

Before diving into the technical details, let’s take a moment to outline the main features of our OS.

## Features in 1K Lines of Code (1K LoC) OS

In this book, we will implement the following major features:

- **Multitasking**: Let multiple applications share the CPU by switching between processes.
- **Exception handler**: Respond to CPU and software events that need kernel intervention, such as illegal instructions.
- **Paging**: Provide each application with its own isolated memory address space.
- **System calls**: Allow applications to request services from the kernel.
- **Device drivers**: Abstract hardware interactions (like disk input/output or read/write).
- **File system**: Organize data on the disk and manage files.
- **Command-line shell**: A simple interface for user input.

## Features not implemented

The following major features are not implemented in this book:

- **Interrupt handling**: Instead of hardware interrupts, we’ll use polling (busy waiting) to check device status.
- **Timer processing**: We’ll use cooperative multitasking, where processes voluntarily yield the CPU. No preemption timer is involved.
- **Inter-process communication**: No pipes, UNIX domain sockets, or shared memory.
- **Multi-processor support**: The OS will only support a single CPU core.

## Source code structure

We’ll build our OS step by step. By the end, our project will hopefully look something like this (*disclaimer*: this is subject to change depending on the direction i end up taking):

```
├── disk/     - File system contents
├── common.c  - Kernel/user common library: printf, memset, ...
├── common.h  - Kernel/user common library: definitions of structs and constants
├── kernel.c  - Kernel: process management, system calls, device drivers, file system
├── kernel.h  - Kernel: definitions of structs and constants
├── kernel.ld - Kernel: linker script (memory layout definition)
├── shell.c   - Command-line shell
├── user.c    - User library: functions for system calls
├── user.h    - User library: definitions of structs and constants
├── user.ld   - User: linker script (memory layout definition)
└── run.sh    - Build script
```

> [!TIP]
>
> In this book, "user land" is sometimes abbreviated as "user". Consider it as "applications", and do not confuse it with "user account"!