FILES = build/kernel.asm.o build/kernel.o
LD = ${PREFIX}/i686-elf-ld
CC = ${PREFIX}/i686-elf-gcc
INCLUDES = -I.src/
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: bin/boot.bin bin/kernel.bin
	rm -rf bin/os.bin
	dd if=bin/boot.bin >> bin/os.bin

bin/boot.bin: src/boot/boot.asm
	nasm -f bin src/boot/boot.asm -o bin/boot.bin

bin/kernel.bin: $(FILES)
	$(LD) -g -relocatable $(FILES) -o build/kernelfull.o
	$(CC) $(FLAGS) -T src/linker.ld build/kernelfull.o -o bin/kernel.bin

build/kernel.asm.o: src/kernel.asm
	nasm -g -f elf src/kernel.asm -o build/kernel.asm.o

build/kernel.o: src/kernel.c
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

clean:
	rm -rf bin/boot.bin
	rm -rf bin/kernel.bin
	rm -rf bin/os.bin
	rm -rf build/kernelfull.o
	rm -rf $(FILES)