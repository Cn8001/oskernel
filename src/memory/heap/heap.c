#include "heap.h"
#include <stdbool.h>
#include "status.h"
#include "memory.h"

static int heap_validate_table(void* ptr,void* end,struct heap_table* table){
    int res = 0;
    size_t table_size = (size_t)(end-ptr);
    size_t total_blocks = table_size / HESOS_HEAP_BLOCK_SIZE;
    if(table->total != total_blocks){
        res = -EINVARG;
        goto out;
    }


out:
    return res;
}

static bool heap_validate_alignment(void* ptr){
    return ((unsigned int)ptr % HESOS_HEAP_BLOCK_SIZE) == 0;
}

static size_t heap_align_to_upper(size_t size){
    if((size % HESOS_HEAP_BLOCK_SIZE) == 0){
        return size;
    }
    size = (size-(size%HESOS_HEAP_BLOCK_SIZE));
    size += HESOS_HEAP_BLOCK_SIZE;

    return size;
}

int heap_create(struct heap* heap,void* start, void* end, struct heap_table* table){
    int res = 0;
    if(!heap_validate_alignment(start) || !heap_validate_alignment(end)){
        res = -EINVARG;
        goto out;
    }
    memset(heap,0,sizeof(struct heap));
    heap->saddr = start;
    heap->table = table;

    res = heap_validate_table(start,end,table);
    if(res < 0){
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries,HEAP_BLOCK_TABLE_ENTRY_FREE,table_size);


out:
    return res;
}
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry){
    return entry & 0x0f; //0x0f bitler çevrildiği için son değil ilk bitleri veriyor.
}
uint32_t heap_get_start_block(struct heap* heap,uint32_t blocks){
    struct heap_table* table = heap->table;
    int bcurrent = 0;
    int bstart = -1;

    //Look for (a) suitable entry/entries
    for(size_t i =0; i<table->total; i++){
        if(heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE){
            bcurrent = 0;
            bstart = -1;
            continue;
        }

        /*If this is the first block (Dolu olanları continue ile attık zaten ve bs hala -1se demek ki bundan önce hiç boş blok yok)*/
        if(bstart == -1){
            bstart = i;
        }
        bcurrent++;
        /* Yeterli sayıda bloğu bulduk (Eğer start block bulduysak ama devamı doluysa doluları atlayacağı için current ile bs aynı olacak)*/
        if(bcurrent == blocks){
            break;
        }
    }
    /*Eğer bsyi hala değiştirmediyse demek ki uygun alan yok*/
    if(bstart == -1){
        return -ENOMEM;
    }

    return bstart;
}

void* heap_block_to_address(struct heap* heap,uint32_t block){
    /* Return origin + block_number * 4096*/
    return heap->saddr + (block * HESOS_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_as_taken(struct heap* heap,int start_block,int total_blocks){
    int end_block = (start_block + total_blocks)-1; //End block index (0 1 2)

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if(total_blocks > 1){
        entry |= HEAP_BLOCK_HAS_NEXT;
    }
    for(int i=start_block;i<=end_block;i++){
        heap->table->entries[i] = entry;

        /* Ilk bloğu mark ettikten sonra first block flagini ve has_nexti kaldırıyoruz.*/
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;

        /*Eğer sonuncu bloğa loop etmeyecekse(çünkü bu set edildikten sonra çalışıyor.) has_next flagini ekliyoruz.*/
        if(i != end_block-1){
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}
void* heap_malloc_blocks(struct heap* heap,uint32_t blocks){
    void* address = 0;
    /*Uygun bir yer bul*/
    uint32_t start_block = heap_get_start_block(heap,blocks);
    if(start_block < 0){
        goto out;
    }
    /*Bloğu fiziksel adrese çevir*/
    address = heap_block_to_address(heap,start_block);
    /* Mark the blocks as taken */
    heap_mark_blocks_as_taken(heap,start_block,blocks);
out:
    return address;
}

void* heap_malloc(struct heap* heap,size_t size){
    size_t aligned_size = heap_align_to_upper(size); // 4096nın katlarına yuvarla

    uint32_t total_blocks = aligned_size / HESOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap,total_blocks);
}

int heap_address_to_block(struct heap* heap,void* address){
    /* Returns block number of address */
    return ((int)(address - heap->saddr)) / HESOS_HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_as_free(struct heap* heap, int starting_block){
    struct heap_table* table = heap->table;
    for(size_t i = starting_block; i < table->total; i++){
        /* Get the entry (Sonda mıyız bakmak için)*/
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        
        /* Entryi boş olarak yap.*/
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        /*Break if we reach to the end*/
        if(!(entry & HEAP_BLOCK_HAS_NEXT)){
            break;
        }
    }
}

void heap_free(struct heap* heap,void* ptr){
    heap_mark_blocks_as_free(heap,heap_address_to_block(heap,ptr));
}

