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
    or al,1
    mov cr0,eax

    ;Jump to 32bit
    ;jmp CODE_SEG:load32

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

times 510-($-$$) db 0       ; 510 bytes of bootloader and 2 bytes of signature
dw 0xaa55                   ; Boot signature



