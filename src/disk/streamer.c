#include "streamer.h"
#include "config.h"
#include "kheap.h"

struct disk_stream* diskstreamer_new(int disk_id){
    struct disk* disk = disk_get(disk_id);
    if(!disk){
        return 0;
    }
    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->disk = disk;
    streamer->pos = 0x0-16; //Normal kodda 16 yok ama bizde 16 byte sonrasını okudupu için -16 koydum
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos){
    stream->pos = pos-16; //Normal kodda 16 yok ama bizde 16 bit sonrasını okuduğu için -16 koydum
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total_bytes){
    int sector_num = stream->pos / HESOS_DISK_SECTOR_SIZE;
    int offset = stream->pos % HESOS_DISK_SECTOR_SIZE;
    char buff[HESOS_DISK_SECTOR_SIZE];

    int res = disk_read_block(stream->disk,sector_num,1,buff);
    if(res < 0){
        goto out;
    }

    //Sadece tek sector okuyabiliriz.

    int total_to_read = total_bytes > HESOS_DISK_SECTOR_SIZE ? HESOS_DISK_SECTOR_SIZE : total_bytes;

    //Sektörü oku ama sadece istenilen byte kadarını outa at.
    for(int i=0; i<total_to_read; i++){
        //Önce koyar sonra artırır. out++ ++out değil. Böylece okumayı bitirince son yer boş oluyor ve tekrar döndüğünde sıkıntı olmuyor.
        *(char*)out++ = buff[offset+i];
    }

    //Positionu arttır.
    stream->pos += total_to_read;

    //Eğer hala okunacak byteler varsa fonksiyonu tekrar çağır. (Her loop 1 sector)
    if(total_bytes > HESOS_DISK_SECTOR_SIZE){
        diskstreamer_read(stream,out,total_bytes-HESOS_DISK_SECTOR_SIZE);
    }


out:
    return res;
}

void diskstreamer_free(struct disk_stream* stream){
    kfree(stream);
}