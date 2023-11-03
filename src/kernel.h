#ifndef KERNEL_H
#define KERNEL_H
typedef struct{
    unsigned short buffer[32768];
}vga_first_buffer;
void print(char *ptr);
void kernel_main();

#endif