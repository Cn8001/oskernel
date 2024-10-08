#include "file.h"
#include "kernel.h"
#include "config.h"
#include "memory.h"
#include "status.h"
#include "kheap.h"
#include "disk.h"
#include "string.h"
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
FILE_MODE file_get_mode_by_string(const char* str){
    FILE_MODE mode = FILE_MODE_INVALID;
    if(strncmp(str,"r",1) == 0){
        mode = FILE_MODE_READ;
    }
    else if(strncmp(str,"w",1) == 0){
        mode = FILE_MODE_WRITE;
    }
    else if(strncmp(str,"a",1) == 0){
        mode = FILE_MODE_APPEND;
    }
    return mode;
}
int fopen(const char* filename, const char* mode_str){
    int res = 0;
    struct path_root* root_path = pathparser_parse(filename,NULL);
    if(!root_path){
        res = -EINVARG;
        goto out;
    }

    //Gelen parametre 0:/ şeklindeyse
    if(!root_path->first){
        res = -EINVARG;
        goto out;
    }

    struct disk* disk = disk_get(root_path->drive_no);

    //Eğer öyle bir disk yoksa
    if(!disk){
        res = -EIO;
        goto out;
    }

    //Eğer diskte bir filesystem yoksa
    if(!disk->filesystem){
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    //Eğer mode olarak gönderilen string r,w,a değilse
    if(mode == FILE_MODE_INVALID){
        res = -EINVARG;
        goto out;
    }
    void* descriptor_private_data = disk->filesystem->open(disk,root_path->first,mode);
    if(ISERR(descriptor_private_data)){
        res = ERROR_I(descriptor_private_data);
        goto out;
    }
    struct file_descriptor* desc = 0;
    res = fs_new_descriptor(&desc);
    if(res < 0)
        goto out;
    desc->filesystem = disk->filesystem;
    desc->disk = disk;
    //Dosya hakkında bilgiler (FAT hakkında değil)
    desc->private = descriptor_private_data;
    res = desc->index;
out:
    //Fopen negative value döndürmemeli
    if(res < 0){
        res = 0;
    }
    return res;
}
