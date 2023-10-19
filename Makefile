FILES = build/kernel.asm.o
LD = ${PREFIX}/i686-elf-ld
CC = ${PREFIX}/i686-elf-gcc

all: bin/boot.bin bin/kernel.bin
	rm -rf bin/os.bin
	dd if=bin/boot.bin >> bin/os.bin

bin/boot.bin: src/boot/boot.asm
	nasm -f bin src/boot/boot.asm -o bin/boot.bin

bin/kernel.bin: $(FILES)
	$(LD) -g -relocatable $(FILES) -o build/kernelfull.o
	$(CC) -g -T src/linker.ld build/kernelfull.o -o bin/kernel.bin -ffreestanding -O0 -nostdlib 

build/kernel.asm.o: src/kernel.asm
	nasm -g -f elf src/kernel.asm -o build/kernel.asm.o

clean:
	rm -rf bin/boot.bin
	rm -rf build/kernel.asm.o