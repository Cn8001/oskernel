#pragma once
#ifndef FILE_H
#define FILE_H

#include "pparser.h"

typedef unsigned int FILE_SEEK_MODE;
enum{
    SEEK_SET,
    SEEK_CURR,
    SEEK_END
};

typedef unsigned int FILE_MODE;

enum{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};
struct disk;
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk,struct path_part* path,FILE_MODE mode); //Böylece FS_OPEN_FUNCTION adında bir fonksiyon pointer türü oluşturuyoruz.
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk); //Böylece fs_resolve_function diye bir tür tanımlıyoruz. (return value için)

struct filesystem{
    //Eğer disk onunsa resolve function 0 döndürür.
    FS_OPEN_FUNCTION open;
    FS_RESOLVE_FUNCTION resolve;

    char name[20];
};

struct file_descriptor{

    //Descriptor index
    int index;
    struct filesystem* filesystem;

    //Private data about file for the fs

    void* private;

    //Descriptorun olduğu disk
    struct disk* disk;
};

void fs_init();
int fopen(const char* filename, const char* mode);
void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);

#endif