#include "common.h"

// You'll need a putchar function somewhere:
void putchar(char ch);  // Typically provided by your environment

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;  // Skip '%'
            
            if (*fmt == '\0') {
                // Edge case: '%' is the last character
                putchar('%');
                break;
            }

            switch (*fmt) {
                case '%':
                    // Print a literal '%'
                    putchar('%');
                    break;

                case 's': {
                    // Print a null-terminated string
                    const char *s = va_arg(args, const char *);
                    if (!s) {
                        s = "(null)";
                    }
                    while (*s) {
                        putchar(*s++);
                    }
                    break;
                }

                case 'd': {
                    // Print a signed integer in decimal
                    int value = va_arg(args, int);
                    if (value == 0) {
                        putchar('0');
                        break;
                    }
                    if (value < 0) {
                        putchar('-');
                        value = -value;
                    }

                    // Determine the highest divisor (1, 10, 100, etc.)
                    int divisor = 1;
                    while ((value / divisor) >= 10) {
                        divisor *= 10;
                    }

                    // Print each digit
                    while (divisor > 0) {
                        putchar('0' + (value / divisor));
                        value %= divisor;
                        divisor /= 10;
                    }
                    break;
                }

                case 'x': {
                    // Print an integer in hexadecimal (8 hex digits)
                    unsigned int value = va_arg(args, unsigned int);
                    for (int i = 7; i >= 0; i--) {
                        unsigned int nibble = (value >> (i * 4)) & 0xF;
                        putchar("0123456789abcdef"[nibble]);
                    }
                    break;
                }

                default:
                    // For unknown specifiers, print them literally
                    putchar('%');
                    putchar(*fmt);
                    break;
            }
        } else {
            // Normal character, just print
            putchar(*fmt);
        }
        fmt++;
    }

    va_end(args);
}