#pragma once
#ifndef KHEAP_H
#define KHEAP_H
#include <stddef.h>

void* kmalloc(size_t size);
void kheap_init();
void kfree(void* ptr);
void* kzalloc(size_t size);

#endif