---
title: Hello World
---

# Hello World in RISC-V

## Overview

In this chapter, we'll modify our kernel to print "Hello, World!" to the console. This requires using OpenSBI's console output functionality through an SBI call.

## What is SBI?

The Supervisor Binary Interface (SBI) provides services that the kernel can use, like console I/O or system shutdown. These services are implemented by the firmware (OpenSBI in our case) and accessed through the `ecall` instruction. We can imagine SBI as an "API" for the OS.

## Making SBI Calls

To make an SBI call, we need to:
1. Put arguments in registers a0-a7
2. Execute `ecall`
3. Get return values from a0-a1

Here's the C struct and function we'll use for SBI calls:

```c
struct sbiret {
    long error;
    long value;
};
```
