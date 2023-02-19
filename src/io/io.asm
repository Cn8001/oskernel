section .asm
global insb
global insw
global outb
global outw

insb:   ; Ebp+8 -> first parameter, Ebp + 4-> Return addres, eax -> Return value
    push ebp
    mov ebp,esp

    xor eax,eax         ; eax'ı 0 yap (temizle)
    mov edx,[ebp+8]     ; in dx,al
    in al,dx            ; Veriyi oku (eax zaten return value yani al deki veriyi C dilinden okuyabiliriz.)

    pop ebp
    ret

insw:
    push ebp
    mov ebp,esp

    xor eax,eax
    mov edx,[ebp+8]
    in ax,dx    ; 16 bit çeker (ax) (Eax zaten return value)

    pop ebp
    ret

outb:
    push ebp
    mov ebp,esp

    mov eax,[ebp+12]    ; 2. parametre -> value
    mov edx,[ebp+8]     ; 1. parametre -> port
    out dx,al

    pop ebp
    ret

outw:
    push ebp
    mov ebp,esp
    mov eax,[ebp+12]
    mov edx,[ebp+8]
    out dx,ax
    pop ebp
    ret


