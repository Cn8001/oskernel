#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "kheap.h"

uint16_t* video_mem = 0;
uint16_t terminal_col;
uint16_t terminal_row;

uint16_t terminal_make_char(uint8_t c,uint8_t color){
    return (color << 8) | c;    // Endian (önce büyük bit girmesi) yüzünden color | char yapıyoruz. Swap ediliyor.
}

void terminal_putchar(int x,int y,char c,char color){
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c,color);
}

void terminal_writechar(char c,char color){
    if(c == '\n'){
        terminal_col = 0;
        terminal_row++;
        return; // \n karakterini atlıyoruz. (print fonksiyonu dönmeye devam ediyor.)
    }
    terminal_putchar(terminal_col,terminal_row,c,color);
    terminal_col++;
    if(terminal_col >= VGA_WIDTH){
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_initialize(){
    terminal_col = 0;
    terminal_row = 0;
    video_mem = (uint16_t*)(0xB8000);
    for(int y = 0; y < VGA_HEIGHT; y++){
        for(int x = 0; x < VGA_WIDTH; x++){
            terminal_putchar(x,y,' ',1);
        }
    }
}

size_t strlen(const char* str){
    size_t len = 0;
    while(str[len]){
        len++;
    }
    return len;
}

void print(const char* str){
    size_t len = strlen(str);
    for(size_t i = 0; i < len; i++){
        terminal_writechar(str[i],15);
    }
}

void kernel_main(){

    terminal_initialize();

    //Initialize the kernel heap
    kheap_init();

    // Initalize the interrupt descriptor table
    idt_init();

    //After initialize the idt enable interrupts
    enable_interrupts(); 

    void* ptr = kmalloc(50);
    void* ptr2 = kmalloc(5000);
    void* ptr4 = kmalloc(5600);
    kfree(ptr);

    void* ptr3 = kmalloc(3000);
    if(ptr || ptr2 || ptr3 || ptr4){

    }

    print("Hello world!\ntest\n\nDeneme");
}