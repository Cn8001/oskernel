section .asm
global idt_load
global enable_interrupts
global disable_interrupts
global noint
global int21h

extern int21h_handler
extern noint_handler

idt_load:
    push ebp    ; EBP'yi sakla
    mov ebp,esp ; Base'yi stack pointere çek. (ebxe ebpden atılıyor.)
    mov ebx,[ebp+8] ; Ebp -> stackteki ebp, Ebp+4 -> Bizi çağıran fonksiyonun adresi, Ebp+8 -> İlk parametre, Ebp+12 -> İkinci parametre
    lidt[ebx]       ; Ebxte ilk parametremiz (idt addres) var onu yükle. (Stackte adres arttıkça veri eskiye gider.)
    pop ebp         ; Restore ebp
    ret

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

noint:
    cli
    pushad
    call noint_handler
    popad
    sti
    iret

int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret