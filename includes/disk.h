#pragma once
#ifndef DISK_H
#define DISK_H

typedef unsigned int HESOS_DISK_TYPE;

#define HESOS_DISK_TYPE_REAL 0

struct disk{
    HESOS_DISK_TYPE type;
    int sector_size;
};

int disk_read_block(struct disk* idisk,unsigned int lba, int total,void* buf);
void disk_search_and_init();
struct disk* disk_get(int number);

#endif