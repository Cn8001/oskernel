#include "idt.h"
#include "config.h"
#include "memory.h"
#include "kernel.h"

extern void idt_load(void* idtr_address);

struct idt_desc idt_descriptors[HESOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;


void idt_zero(){
    print("Divide by Zero error!\n");
}

void idt_set(int interrupt_no, void* address){
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;   // First 16 bits of address
    desc->selector = KERNEL_CODE_SELECTOR;       // Code segment selector
    desc->zero = 0x00;
    desc->type_attr = 0xEE;                     // Type,gate etc. bits
    desc->offset_2 = (uint32_t)address >> 16;   // High 16 bits of address
}
void idt_init(){
    memset(idt_descriptors,0,sizeof(idt_descriptors));
    idtr_descriptor.size = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

    idt_set(0,idt_zero);

    // Load the IDT
    idt_load(&idtr_descriptor);
}