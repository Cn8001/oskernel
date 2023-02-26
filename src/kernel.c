#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "kheap.h"
#include "paging.h"
#include "disk.h"
#include "pparser.h"
#include "string.h"

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

void print(const char* str){
    size_t len = strlen(str);
    for(size_t i = 0; i < len; i++){
        terminal_writechar(str[i],15);
    }
}
static struct paging_4gb_chunk* kernel_chunk = 0;
void kernel_main(){

    terminal_initialize();

    //Initialize the kernel heap
    kheap_init();

    // Initalize the interrupt descriptor table
    idt_init();

    //Search and initialize disks
    disk_search_and_init();

    //Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_PAGE_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    //Switch to kernel chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    //Enable paging
    enable_paging();

    //After initialize the idt enable interrupts
    enable_interrupts(); 

    struct path_root* root_path2 = pathparser_parse("0:/ppad/shell.txt",NULL);
    if(root_path2){

    }


    print("Hello world!\ntest\n\nDeneme");
}