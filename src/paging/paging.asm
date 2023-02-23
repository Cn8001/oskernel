[BITS 32]
section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp,esp
    mov eax,[ebp+8]     ; Paging directory eaxte
    mov cr3,eax         ; CR3 page_directorynin adresini tutmalı.
    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp,esp
    mov eax,cr0         ;CR0ı direkt değiştiremeyiz, o yüzden eaxe atıyoruz.
    or eax,0x80000000   
    mov cr0,eax         ;Enable paging
    pop ebp
    ret

