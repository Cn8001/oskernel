ORG 0x7c00                   ; Set offset 0 
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

bios_parameter_block:
    jmp short _start
    nop

times 33 db 0

_start:
                        ; At this point cs segment could not be correct, so we use value of the segment instead of register. 
                        ;(This also changes cs to 0x7c0)
    jmp 0:start    

start:
    cli
    ; Setup segment registers
    mov ax,0x00
    mov ds,ax
    mov es,ax
    ;Stack grows downwards
    mov ss,ax
    mov sp,0x7c00           ;Stack pointer should point top of the stack 0x7c00 -> 0x000'a doğru gidiyor.
    sti

.load_protected:
    cli         ; Disable interrupts

    ;Load GDT
    lgdt[gdt_descriptor]

    ;Set cr0 bit
    mov eax,cr0
    or eax,1
    mov cr0,eax

    ;Jump to 32bit
    jmp CODE_SEG:load32

; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8 -> code descriptor (CS)
gdt_code:
    dw 0xffff               ; Segment limit 0-15
    dw 0                    ; Base first 0-15 bits
    db 0                    ; Base 16-23 bits
    db 0x9a                 ; Access byte (bitmask)
    db 11001111b            ; High 4 bit flags, low 4 bit flags
    db 0                    ; Base 24-31 bits

; offset 0x10 -> data descriptor (DS,SS,ES,GS,FS)
gdt_data:
    dw 0xffff               ; Segment limit 0-15
    dw 0                    ; Base first 0-15 bits
    db 0                    ; Base 16-23 bits
    db 0x92                 ; Access byte (bitmask)
    db 11001111b            ; High 4 bit flags, low 4 bit flags
    db 0                    ; Base 24-31 bits
gdt_end:

;GDTR
gdt_descriptor:
    dw gdt_end - gdt_start-1;Size
    dd gdt_start            ; Offset

[BITS 32]
 load32:
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax, ; Backup the LBA
    ; Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select the  master drive
    mov dx, 0x1F6
    out dx, al
    ; Finished sending the highest 8 bits of the lba

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    ; Send more bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory
.next_sector:
    push ecx

; Checking if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

; We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret

times 510-($-$$) db 0       ; 510 bytes of bootloader and 2 bytes of signature
dw 0xaa55                   ; Boot signature



