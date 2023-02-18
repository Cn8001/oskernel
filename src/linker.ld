ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M; /*Dosyaların orijini (Böylece bootloader kerneli bulup zıplayacak)*/
    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(COMMON)
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
    
}