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


    ;CHS disk okuma başlangıcı
    mov ah,2                     ; Okuma modu
    mov al,1                        ; Number of sectors to read
    mov ch,0                        ; Cyclinder number (Low 8 bits of CHS)
    mov dh,0                        ; Head number (two bits of CHS) (DL drive numberi bios bootta dlye koyuyor.)
    mov cl,2                        ; Sector number (2. sektörü okumak istiyoruz.)
    mov bx,buffer                   ; Where to put datas (0x7c0 * 16 + offset)
    int 0x13                        ; Disk interruptu
    jc error                        ; Eğer carry flag (cf) 0 değilse atlar.
    ; Cyclinder, head and sector okuma bitişi
   
    mov si,buffer
    call print
    jmp $

error:
    mov si,error_message
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

error_message: db 'Disk read error',0   ; Eğer 1. sektör dışında olursa bunu okuyor.

times 510-($-$$) db 0
dw 0xAA55

buffer: ;Diskten okunan verinin konulacağı yer (cs:offset) (0x7c0 * 16 + 200)