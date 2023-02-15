ORG 0
BITS 16

_start:
    jmp short start ; BIOS Parameter block ilk üç byteyi böyle istiyor.
    nop
times 33 db 0 ; BPB için byteler


start:
    jmp 0x7c0:step2         ; cs:start'e gittiği için cs -> 0x7c0 oluyor (segmentler 16 ile çarpılır.) cs yi ayarlıyoruz.

step2:
    cli                 ;Segmentleri ayarlarken interruptla kesmesin
    mov ax,0x7c0        ; ax * 16 + offset (16 ile çarpılıyor diye)
    mov ds,ax
    mov es,ax
    mov ax,0x00         ; Stack segment 0 * 16 + stack_pointer (0+sp)
    mov ss,ax
    mov sp,0x7c00      ; Bu 16 ile çarpılmıyor.
    sti

    mov si,message
    call print
    jmp $

print:
    mov bx,0
.loop:
    lodsb
    cmp al,0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah,0eh
    int 0x10
    ret


message: db 'Hello world!',0
times 510-($-$$) db 0
dw 0xAA55