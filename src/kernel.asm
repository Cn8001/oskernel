[BITS 32]

CODE_SEG equ 0x8
DATA_SEG equ 0x10

extern kernel_main
global _start

_start:
    ; Set data segments
    mov ax,DATA_SEG
    mov ds,ax
    mov ss,ax
    mov gs,ax
    mov es,ax
    mov fs,ax

    mov ebp,0x200000      ; Put base and stack away from bootloader
    mov esp,ebp

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    call kernel_main
    jmp $

times 512-($-$$) db 0