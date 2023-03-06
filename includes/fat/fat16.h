#pragma once
#ifndef FAT16_H
#define FAT16_h
#include "file.h"
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk,struct path_part* path,FILE_MODE mode); //Böylece FS_OPEN_FUNCTION adında bir fonksiyon pointer türü oluşturuyoruz.
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk); //Böylece fs_resolve_function diye bir tür tanımlıyoruz. (return value için)
struct filesystem* fat16_init();

#endif