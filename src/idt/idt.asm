section .asm
global idt_load

idt_load:
    push ebp    ; EBP'yi sakla
    mov ebp,esp ; Base'yi stack pointere çek. (ebxe ebpden atılıyor.)
    mov ebx,[ebp+8] ; Ebp -> stackteki ebp, Ebp+4 -> Bizi çağıran fonksiyonun adresi, Ebp+8 -> İlk parametre, Ebp+12 -> İkinci parametre
    lidt[ebx]       ; Ebxte ilk parametremiz (idt addres) var onu yükle. (Stackte adres arttıkça veri eskiye gider.)
    pop ebp         ; Restore ebp
    ret