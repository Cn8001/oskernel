#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct idt_desc{
    uint16_t offset_1;            // Offset 0-15 bits
    uint16_t selector;           // CS from our GDT
    uint8_t zero;
    uint8_t type_attr;          //Descriptor type and attributes (trap gate/interrupt, privilieged interrupt etc.)
    uint16_t offset_2;

}__attribute__((packed));

struct idtr_desc{
    uint16_t size;              // Size of idt -1
    uint32_t base;            // Starting offset of idt
}__attribute__((packed));

void idt_init();

#endif