#include "kernel.h"

vga_first_buffer buffer;
int index=0;

void draw_chars(){
    unsigned short *ptr = (unsigned short*)0xb8000;
    for(int i=0;i<sizeof(buffer.buffer)/2;i++){
        if(buffer.buffer[i]){
            unsigned short val = buffer.buffer[i];
            ptr[i] = val;
            continue;
        }
        ptr[i] = 0;
    }
}

void init_screen(){
    for (int i = 0; i <sizeof(buffer.buffer)/2; i++)
    {
        buffer.buffer[i] = 0;
    }
    
}

void putc(unsigned char colour, unsigned char ascii){
    if(index<sizeof(buffer.buffer)/2){
        buffer.buffer[index++] = (colour << 8) | ascii;
    }
}

void print(char *ptr){
    for (int i=0;ptr[i] != '\0'; i++)
    {
        putc(7,ptr[i]);
    }
    
}

void kernel_main(){
    init_screen();
    print("Hello world!");
    draw_chars();
}