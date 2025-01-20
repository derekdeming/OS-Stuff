OUTPUT_ARCH(riscv)
ENTRY(_start);

SECTIONS
{
    /* Move this from 0x80000000 to 0x80200000 (or any safe address > 0x80041818) -- got message that some ROM regions are overlapped */
    . = 0x80300000;

    .text : {
        *(.text*)
    }
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
    }
    .bss : {
        *(.bss*) *(COMMON)
    }
    . = ALIGN(8);
    __bss_end = .;
    _end = .;
}