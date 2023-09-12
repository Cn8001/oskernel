ORG 0                   ; Set offset 0 
BITS 16

bios_parameter_block:
    jmp short _start
    nop

times 33 db 0

_start:
    ; At this point cs segment could not be correct, so we use value of the segment instead of register. 
    ;(This also changes cs to 0x7c0)
    jmp 0x7c0:start    

start:
    cli
    ; Setup segment registers
    mov ax,0x7c0
    mov ds,ax
    mov es,ax
    
    ;Stack grows downwards
    mov ax,0x00
    mov ss,ax

    mov sp,0x7c00   ;Stack pointer should point top of the stack 0x7c00 -> 0x000'a doğru gidiyor.
    sti

    ; Read sector 2 into memory
    call read_from_disk

    ; Test wheter it loaded 
    mov si,buffer
    call print

    jmp $                   ; We cant ret to the OS bcz we are the kernel

read_from_disk:
    mov ah,02h              ; Read sector command
    mov al,1                ; Read 1 sector
    mov ch,0                ; Low cyclinder number
    mov cl,2                ; Sector number 
    mov dh,0                ; Head number
    mov bx,buffer           ; es:bx -> where to load
    int 0x13
    jc error                ; Jump if carry is set
    ret
error:
    mov si,error_message
    call print
    jmp $

print:
    mov bx,0
    mov ah,0eh
._loop:
    lodsb
    cmp al,0
    je .done
    int 10h
    jmp ._loop
.done:
    ret

error_message:
    db 'Failed to load sector',0

times 510-($-$$) db 0       ; 510 bytes of bootloader and 2 bytes of signature
dw 0xaa55                   ; Boot signature

buffer:                     ; BIOS doesn't load this into memory we just reference a memory address like a pointer. Not a disk address. 



