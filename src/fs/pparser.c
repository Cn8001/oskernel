#include "pparser.h"
#include "kernel.h"
#include "string.h"
#include "kheap.h"
#include "config.h"
#include "memory.h"
#include "status.h"

static int pathparser_path_valid_format(const char* filename){
    int len = strnlen(filename,HESOS_MAX_PATH);
    return (len >=3 && isdigit(filename[0]) && memcmp((void*)&filename[1],":/",2) == 0); // 0:/, 1:/
}

static int pathparser_get_drive_by_path(const char** path){
    if(!pathparser_path_valid_format(*path)){
        return -EBADPATH;
    }

    int drive_no = tonumericdigit(*path[0]);

    // Pathin değerini 3 artırıyoruz ki drive_noyu atlasın. (Bu yüzden çift pointer aldık.)
    *path+=3;
    return drive_no;
}


static struct path_root* pathparser_create_root(int drive_no){
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_no;
    path_r->first = 0;
    return path_r;
}

static const char* pathparser_get_path_part(const char** path){
    char* result_path_part = kzalloc(HESOS_MAX_PATH);
    int i =0;
    while(**path != '/' && **path != 0x00){
        result_path_part[i] = **path;
        *path+=1; //Böylece gönderilen parametrenin de değeri değişiyor test/test.txt -> /test.txt oluyor.
        i++;
    }

    if(**path == '/'){
        //Eğer bu karakteri görürse looptan çıkar ama bu karakter kalıyor. Sonraki okumada sıkıntı çıkarır.
        *path+=1;
    }

    if(i == 0){
        //Eğer i hala 0sa demek ki yeni path yok
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

struct path_part* pathparser_parse_path_part(struct path_part* previous,const char** path){
    const char* path_part_str = pathparser_get_path_part(path);
    
    //Eğer bir path yoksa 0 döndür
    if(!path_part_str){
        return 0;
    }

    // Yeni bir part oluştur.
    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;

    //Eğer önceki part verildiyse sonuna ekle (Verilmediyse demek ki ilk partı oluşturuyoruz.)
    if(previous){
        previous->next = part;
    }

    return part;
}

void pathparser_free(struct path_root* root){
    struct path_part* part = root->first;

    //Part dolu olduğu sürece sonraki partı al, sonraki partı sakla, kendi pathini ve partını sil, partı sonrakine ata.
    while(part){
        struct path_part* next_part = part->next;
        kfree((void*)part->part);
        kfree(part);
        part = next_part;
    }
    kfree(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory_path){
    int res = 0;

    //Orijinali korunsun diye kopyaladık.
    const char* tmp_path = path;
    struct path_root* path_root = 0;
    if(strlen(path) > HESOS_MAX_PATH){
        goto out;
    }

    //TMPnin değişmesini istiyoruz o yüzden tmpnin adresini verdik.
    res = pathparser_get_drive_by_path(&tmp_path);
    if(res < 0){
        goto out;
    }
    path_root = pathparser_create_root(res);
    if(!path_root){
        goto out;
    }

    //Ilk partı oluşturuyoruz.
    struct path_part* first_part = pathparser_parse_path_part(NULL,&tmp_path);
    if(!first_part){
        goto out;
    }

    //Roota ilk partı verdik.
    path_root->first = first_part;

    //Partları birleştiriyoruz.
    struct path_part* part = pathparser_parse_path_part(first_part,&tmp_path);
    while(part){
        part = pathparser_parse_path_part(part,&tmp_path);
    }
out:
    return path_root;
}