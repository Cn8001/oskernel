ORG 0x7c00
BITS 16

CODE_SEG equ 0x8
DATA_SEG equ 0x10

_start:
    jmp short start ; BIOS Parameter block ilk üç byteyi böyle istiyor.
    nop

times 33 db 0 ; BPB için byteler


start:
    jmp 0:step2         ; cs:start'e gittiği için cs -> 0x7c0 oluyor (segmentler 16 ile çarpılır.) cs yi ayarlıyoruz.

step2:
    ; Segmentation (BIOS kendi kafasına göre segment atabilir bu yüzden orjin 0 ve segmentleri biz ayarlıyoruz.)
    cli                 ;Segmentleri ayarlarken interruptla kesmesin
    mov ax,0x00        ; ax * 16 + offset (16 ile çarpılıyor diye)
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov sp,0x7c00      ; Bu 16 ile çarpılmıyor.
    sti
    ; End of segmentation

load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax,cr0
    or eax,0x1
    mov cr0,eax
    jmp CODE_SEG:load32 ; GDT_CODE code segmentin olduğu yer ve böyle yapınca cs'yi 0x8 yapıyor ama DS yi elle yapmalıyız

gdt_start:
gdt_null: ; 64 bit 0
    dd 0x0
    dd 0x0
;offset 0x8 (8 byte)
gdt_code:   ; CS should point this
    dw 0xffff       ;Segment limit first 0-15 bits (2 byte)
    dw 0            ;Base BITS 0-15                (2 byte)
    db 0            ;BASE 16-23 bits                (1 byte)
    db 0x9a         ;ACCESS Byte
    db 11001111b    ;HIGH 4 bit flags and low 4 bit flags
    db 0            ;BASE 24-31 bits
;offset 0x10 (8byte)
gdt_data:   ; DS,SS,ES,FS,GS should point this
    dw 0xffff       ;Segment limit first 0-15 bits (2 byte)
    dw 0            ;Base BITS 0-15                (2 byte)
    db 0            ;BASE 16-23 bits                (1 byte)
    db 0x92         ;ACCESS Byte
    db 11001111b    ;HIGH 4 bit flags and low 4 bit flags
    db 0            ;BASE 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start -1
    dd gdt_start
[BITS 32]
load32:
    ;When we jumped cs automaticly set to CODE_SEG (aka GDT_CODE aka 0x8) BUT DS HAVEN'T SET
    mov ax,DATA_SEG
    mov ds,ax
    mov ss,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ebp,0x00200000  ; Base pointer
    mov esp,ebp         ; Stack pointer starts the same address with base but grows
    ; End of setting data etc. selectors

    ; Starting to enable A20
    in al, 0x92
    or al, 2
    out 0x92, al
    ; End of enabling A20


    jmp $
times 510-($-$$) db 0
dw 0xAA55