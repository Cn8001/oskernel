#pragma once
#ifndef FAT16_H
#define FAT16_h
#include "file.h"

#define HESOS_FAT16_SIGNATURE   0x29
#define HESOS_FAT16_ENTRY_SIZE  0x02
#define HESOS_FAT16_BAD_SECTOR  0xff7
#define HESOS_FAT16_UNUSED      0x00

typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk,struct path_part* path,FILE_MODE mode); //Böylece FS_OPEN_FUNCTION adında bir fonksiyon pointer türü oluşturuyoruz.
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk); //Böylece fs_resolve_function diye bir tür tanımlıyoruz. (return value için)
struct filesystem* fat16_init();

#endif