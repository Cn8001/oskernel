ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start ; BIOS Parameter block ilk üç byteyi böyle istiyor.
nop

OEMIdentifier           db 'HESOS   '
BytesPerSector          dw 0x200
SectorsPerCluster       db 0x80
ReservedSectors         dw 200
FATCopies               db 0x02
RootDirEntries          dw 0x40
NumSectors              dw 0x00
MediaType               db 0xF8
SectorsPerFat           dw 0x100
SectorsPerTrack         dw 0x20
NumberOfHeads           dw 0x40
HiddenSectors           dd 0x00
SectorsBig              dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber             db 0x80
WinNTBit                db 0x00
Signature               db 0x29
VolumeID                dd 0xD105
VolumeIDString          db 'HESOS  BOOT'
SystemIDString          db 'FAT16   '


start:
    jmp 0:step2         ; cs:start'e gittiği için cs -> 0 oluyor (segmentler 16 ile çarpılır.) cs yi ayarlıyoruz.

step2:
    ; Segmentation (BIOS kendi kafasına göre segment atabilir bu yüzden orjin 0 ve segmentleri biz ayarlıyoruz.)
    cli                 ;Segmentleri ayarlarken interruptla kesmesin
    mov ax,0x0        ; ax * 16 + offset (16 ile çarpılıyor diye)
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov sp,0x7c00      ; Bu 16 ile çarpılmıyor.
    sti
    ; End of segmentation
.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax,cr0
    or eax,0x1
    mov cr0,eax
    jmp CODE_SEG:preload32

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
preload32:
    mov eax,1           ; Starting sector (0. sektör -> Bootloader)
    mov ecx,100        ; Okunacak sektör sayısı
    mov edi,0x0100000   ; 1M'ye yükle (Yükleme orjini) (insw)
    call ata_lba_read
    jmp CODE_SEG:0x0100000  ; Kerneli buraya yükledik (GDT_CODE:1M)

ata_lba_read:
    mov ebx,eax         ; Backup LBA
    ; Send highest 8 bits of LBA to controller
    shr eax,24          ; 24 bit sağa kaydır
    or eax,0xE0         ; Select the master drive
    mov dx,0x1F6        ; Port
    out dx,al           ; Send 8 bits to the port
    ; Finish sending highest 8 bits of LBA

    ; Kaç sektör okuyacağımızı gönder
    mov eax,ecx
    mov dx,0x1F2    ; Our port
    out dx,al       ; Send total sectors to the port
    ; End of sending total sector count

    ; Send more bits of the LBA
    mov eax,ebx     ; Restore the LBA
    mov dx,0x1F3
    out dx,al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx,0x1F4
    mov eax,ebx     ; Restore the LBA
    shr eax,8       ; 8 bit sağa kaydır. (32 >> 8 -> 24)
    out dx,al
    ; Finish sendin more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx,0x1F5
    mov eax,ebx     ; Restore the LBA
    shr eax,16
    out dx,al
    ; Finish sending upper 16 bits of the LBA

    mov dx,0x1f7
    mov al,0x20
    out dx,al

    ; Read all sectors into memory
.next_sector:
    push ecx

;Checking if we need to read
.try_again:
    mov dx,0x1f7
    in al,dx
    test al,8
    jz .try_again

; We need to read 256 words (512 bytes) at one time
    mov ecx,256
    mov dx,0x1f0
    rep insw    ; insw -> 16 bit okur. rep -> ecx kadar tekrar eder. 
    ;edi ile işaretli alana (1M) atıyor. Böylece kernel 1M ye yükleniyor, linker de buna göre adresleme yapıyor.
    pop ecx
    loop .next_sector   ; Ecx kadar tekrar eder.
    ; End of reading sectors into memory
    ret

times 510-($-$$) db 0
dw 0xAA55