ORG 0
BITS 16

_start:
    jmp short start ; BIOS Parameter block ilk üç byteyi böyle istiyor.
    nop
times 33 db 0 ; BPB için byteler


start:
    jmp 0x7c0:step2         ; cs:start'e gittiği için cs -> 0x7c0 oluyor (segmentler 16 ile çarpılır.) cs yi ayarlıyoruz.


handle_zero:
    mov ah,0eh
    mov bx,0
    mov al,'D'
    int 0x10
    iret

step2:


    ; Segmentation (BIOS kendi kafasına göre segment atabilir bu yüzden orjin 0 ve segmentleri biz ayarlıyoruz.)
    cli                 ;Segmentleri ayarlarken interruptla kesmesin
    mov ax,0x7c0        ; ax * 16 + offset (16 ile çarpılıyor diye)
    mov ds,ax
    mov es,ax
    mov ax,0x00         ; Stack segment 0 * 16 + stack_pointer (0+sp)
    mov ss,ax
    mov sp,0x7c00      ; Bu 16 ile çarpılmıyor.
    sti
    ; End of segmentation







    /* Interrupt vector table -> 0x00 adresinde başlar (0x0 * 16 + 0x00 => 0)*/
    mov word[ss:0x00],handle_zero   ; Ilk 2 byte offset
    mov word[ss:0x02],0x7c0         ; Son 2 byte SEGMENT (segment *16 + offset)(Bır interrupt 4 byte)

    int 0x00                        ; 0x0 * 16 + (0x00) (Interrupt adresi hesaplama)
    /*End of interrupt vector table*/





   
    jmp $
times 510-($-$$) db 0
dw 0xAA55