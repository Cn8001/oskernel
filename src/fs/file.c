#include "file.h"
#include "kernel.h"
#include "config.h"
#include "memory.h"
#include "status.h"
#include "kheap.h"
#include "fat/fat16.h"

struct filesystem* filesystems[HESOS_MAX_FILESYSTEMS];
struct file_descriptor* filedescriptors[HESOS_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem(){
    for(int i = 0; i<HESOS_MAX_FILESYSTEMS;i++){
        if(filesystems[i] == 0){
            return &filesystems[i];
        }
    }
    return 0;
}

void fs_insert_filesystem(struct filesystem* filesystem){
    struct filesystem** fs = fs_get_free_filesystem();
    if(!fs){
        print("Problem while inserting filesystem!\n");
        while(1){}
    }

    *fs = filesystem;
}

static void fs_static_load(){
    fs_insert_filesystem(fat16_init());
}

void fs_load(){
    memset(filesystems,0,sizeof(filesystems));
    fs_static_load();
}

void fs_init(){
    memset(filedescriptors,0,sizeof(filedescriptors));
    fs_load();
}

//Fills and adds to the list to the given file_descriptor and returns its two dimensional pointer
static int fs_new_descriptor(struct file_descriptor** desc_out){
    int res = -ENOMEM;
    for(int i=0; i<HESOS_MAX_FILE_DESCRIPTORS;i++){
        if(filedescriptors[i] == 0){
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));

            //Descriptorlar 1den başlar
            desc->index = i+1;
            filedescriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }
    return res;
}

static struct file_descriptor* file_get_descriptor(int fd){
    if(fd <= 0 || fd >= HESOS_MAX_FILE_DESCRIPTORS){
        return 0;
    }

    int index = fd-1; //Filedescriptorlar 1 den başlar ama arraydeki indexi 0dan
    return filedescriptors[index];
}

struct filesystem* fs_resolve(struct disk* disk){
    struct filesystem* fs = 0;
    for(int i=0; i<HESOS_MAX_FILESYSTEMS;i++){
        if(filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0){
            fs = filesystems[i];
            break;
        }
    }
    return fs;
}

int fopen(const char* filename, const char* file_mode){
    return -EIO;
}
