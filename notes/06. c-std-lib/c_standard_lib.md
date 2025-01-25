# C Standard Library
---
title: Hello World
---

The C Standard Library provides a set of standard functions and macros that are used for various purposes such as input/output operations, memory management, string manipulation, mathematical computations, and more. These functions and macros are defined in various header files. We'll implement basic types, string manipulation, and memory operations ourselves. In the real world, we would use the standard library provided by the compiler, but for this project, we'll implement our own.

## Basic Types

This will be stored in `common.h`; some of the more common types are:

```c
typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

#define true  1
#define false 0
#define NULL  ((void *) 0)
#define align_up(value, align)   __builtin_align_up(value, align)
#define is_aligned(value, align) __builtin_is_aligned(value, align)
#define offsetof(type, member)   __builtin_offsetof(type, member)
#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
void printf(const char *fmt, ...);
```
Most of these are macros that are defined in the compiler's header files. We have added a few functions that i've found to be useful: 

- `paddr_t`: A type representing physical memory addresses.
- `vaddr_t`: A type representing virtual memory addresses. Equivalent to `uintptr_t` in the standard library.
- `align_up`: Rounds up value to the nearest multiple of align. align must be a power of 2.
- `is_aligned`: Checks if value is a multiple of align. align must be a power of 2.
- `offsetof`: Returns the offset of a member within a structure (how many bytes from the start of the structure).
- `align_up` and `is_aligned` are useful when dealing with memory alignment. For example, `align_up(0x1234, 0x1000)` returns `0x2000`. Also, `is_aligned(0x2000, 0x1000)` returns true, but `is_aligned(0x2f00, 0x1000)` is false. 
- `va_list`, `va_start`, `va_end`, and `va_arg` are used for variable argument lists.

the functions starting with `__builtin_` are built-in functions (clang specific extensions) that are defined in the compiler's header files.

> [!TIP]
> These macros can also be implemented in pure C without using the compiler's built-in functions. This will deepen your understanding of how they work and improve your C programming skills. The one that i found to be interesting is `align_up` and `is_aligned` because they are used to align memory addresses to the nearest multiple of a power of 2. Then there is `offsetof` which is used to get the offset of a member within a structure. 

### Commonly Used Header Files

- `stdio.h`: Standard Input/Output functions (e.g., `printf`, `scanf`, `fopen`, `fclose`)
- `stdlib.h`: Standard Library functions (e.g., `malloc`, `free`, `atoi`, `rand`)
- `string.h`: String handling functions (e.g., `strcpy`, `strcat`, `strlen`, `strcmp`)
- `math.h`: Mathematical functions (e.g., `sin`, `cos`, `tan`, `sqrt`, `pow`)
- `ctype.h`: Character handling functions (e.g., `isalpha`, `isdigit`, `isspace`, `toupper`, `tolower`)

### Example: Using `printf` and `scanf`

The `printf` function is used to print formatted output to the standard output (usually the console). The `scanf` function is used to read formatted input from the standard input (usually the keyboard).

--- 

### Memory Operations

The `memset` function is used to set a block of memory to a specific value. The `memcpy` function is used to copy a block of memory from one location to another.

More specifically, the `memcpy` function copies `n` bytes from memory area `src` to memory area `dst`. The memory areas must not overlap.

This can be implemented in pure C as follows in this common.c file: 

```c
void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *) dst;
    const uint8_t *s = (const uint8_t *) src;
    while (n--)
        *d++ = *s++;
    return dst;
}
```

the `memset` function is used to set a block of memory to a specific value. The `memcpy` function is used to copy a block of memory from one location to another.

More specifically, the `memset` function copies `n` bytes of value `c` (converted to an unsigned char) to the first `n` bytes pointed to by `s`. So it copies `n` bytes from `src` to `dst`.

This can be implemented in pure C as follows in this common.c file: 

```c
void *memset(void *buf, char c, size_t n) {
    char *p = (char *)buf;
    while (n--) {
        *p++ = c;
    }
    return buf;
}
```