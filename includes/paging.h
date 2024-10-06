#pragma once
#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include <stdbool.h>

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE         4096

#define PAGING_PAGE_IS_PRESENT  0b0000001
#define PAGING_IS_WRITABLE      0b0000010
#define PAGING_ACCESS_FROM_ALL  0b0000100
#define PAGING_WRITE_TROUGH     0b0001000   //Write-trough caching or back-caching
#define PAGING_CACHE_DISABLED   0b0010000

struct paging_4gb_chunk{
    uint32_t* directory_entry;
};

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void paging_switch(uint32_t* dir);
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk_4gb);
void enable_paging();
int paging_set(uint32_t* dir,void* virtual_addr,uint32_t val);
bool paging_aligned(void* addr);
#endif