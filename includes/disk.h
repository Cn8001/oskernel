#pragma once
#ifndef DISK_H
#define DISK_H

typedef unsigned int HESOS_DISK_TYPE;

#define HESOS_DISK_TYPE_REAL 0

#include "file.h"

struct disk{
    int id;
    HESOS_DISK_TYPE type;
    int sector_size;
    struct filesystem* filesystem; //Diskte hangi fsnin kullanıldığı

    void* private;      //Private data of our filesystem (Her diskin ayrı bir root directorysi var o yüzden fste değil diskte tutuyoruz.)
};

int disk_read_block(struct disk* idisk,unsigned int lba, int total,void* buf);
void disk_search_and_init();
struct disk* disk_get(int number);

#endif