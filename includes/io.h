#ifndef IO_H
#define IO_H

unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

void outb(unsigned char val, unsigned char port);
void outw(unsigned short val, unsigned short port);

#endif