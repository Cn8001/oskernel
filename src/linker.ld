ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M; /*Dosyaların orijini (Böylece bootloader kerneli bulup zıplayacak)*/
    .text : ALIGN(4096)
    {
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(COMMON)
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(.bss)
    }
    
    .asm : ALIGN(4096)
    {
        *(.asm)
    }
}