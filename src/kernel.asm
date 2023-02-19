[BITS 32]
global _start
extern kernel_main
DATA_SEG equ 0x10
_start:
    ;When we jumped cs automaticly set to CODE_SEG (aka GDT_CODE aka 0x8) BUT DS HAVEN'T SET
    mov ax,DATA_SEG
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ebp,0x00200000  ; Base pointer (Kernelden uzak bir yere eşitle)
    mov esp,ebp         ; Stack pointer starts the same address with base but grows
    ; End of setting data etc. selectors

    ; Starting to enable A20
    in al, 0x92
    or al, 2
    out 0x92, al
    ; End of enabling A20

    ; mov word[0xb8000],0x0241 -> VGA memory

    call kernel_main

    jmp $

times 512-($-$$) db 0 ; To avoid alignment issues we must make the kernel.asm divisible by 16