#include "paging.h"
#include "kheap.h"
#include "status.h"
static uint32_t* current_dir = 0;

extern void paging_load_directory(uint32_t* dir);

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags){
    /* Initialize paging directory */
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    uint32_t offset = 0;
    /* Fill the directory with tables*/
    for(int i=0;i<PAGING_TOTAL_ENTRIES_PER_TABLE; i++){
        /* Initialize paging tables */
        uint32_t* paging_table = (uint32_t*) kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        /* Fill the tables with entries*/
        for(uint32_t b=0;b<PAGING_TOTAL_ENTRIES_PER_TABLE;b++){
            /* Tabloları memory adresleriyle dolduruyoruz (offset + block_number*block_size) ANCAK 20bit yeterli olduğundan diğer bitleri flag olarak kullanıyoruz.*/
            paging_table[b] = (offset + (b*PAGING_PAGE_SIZE)) | flags;
        }
        
        /* Page tablenin offsetini artır.*/
        offset += (PAGING_PAGE_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE);

        /* Directoryi set et*/
        directory[i] = (uint32_t) paging_table | flags | PAGING_IS_WRITABLE;
    }

    struct paging_4gb_chunk* chunk4_gb = (struct paging_4gb_chunk*) kzalloc(sizeof(struct paging_4gb_chunk));
    chunk4_gb->directory = directory;
    return chunk4_gb; //Adresleri tutan directory ve table kernel heapinde. Büyük 4gblık kısım kernel heapinde değil sadece adreslenmiş.
}

void paging_switch(uint32_t* dir){
    paging_load_directory(dir);
    current_dir = dir;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk_4gb){
    return chunk_4gb->directory;
}

static bool paging_aligned(void* addr){
    return ((uint32_t)addr%PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_addr,uint32_t* dir_index_out, uint32_t* table_index_out){
    int res = 0;
    if(!paging_aligned(virtual_addr)){
        res = -EINVARG;
        goto out;
    }
    *dir_index_out = ((uint32_t)virtual_addr / (PAGING_PAGE_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE));
    *table_index_out = ((uint32_t)virtual_addr % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

int paging_set(uint32_t* dir,void* virtual_addr,uint32_t val){
    if(!paging_aligned(virtual_addr)){
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    /* Sanal adrese denk gelen table ve dir numarasını bulup ona hangi adresi işaret edeceğini göstereceğiz.*/
    int res = paging_get_indexes(virtual_addr,&directory_index,&table_index);
    if(res < 0){
        return res;
    }
    uint32_t table_entry = dir[directory_index];
    uint32_t* table = (uint32_t*)(table_entry & 0xfffff000); //Ilk 20 bit address son 12 bit flag (tablonun adresini buluyoruz.)
    table[table_index] = val;
    return 0;
}