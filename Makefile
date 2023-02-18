FILES = ./build/kernel.asm.o
FLAGS = -ffreestanding -O0 -nostdlib
all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin
	
./bin/boot.bin: ./src/boot/boot.asm #Bootloader binarysi çıkarıyor.
	nasm -f bin src/boot/boot.asm -o bin/boot.bin

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld  $(FLAGS) ./build/kernelfull.o -o ./bin/kernel.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o


clean:
	rm -rf bin/boot.bin
	rm -rf bin/kernel.bin
	rm -rf bin/os.bin
	rm -rf ${FILES}
	rm -rf build/kernelfull.o
	