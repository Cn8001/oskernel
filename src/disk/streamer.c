#include "streamer.h"
#include "config.h"
#include "kheap.h"
#include "kernel.h"

struct disk_stream* diskstreamer_new(int disk_id){
    struct disk* disk = disk_get(disk_id);
    if(!disk){
        return 0;
    }
    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->disk = disk;
    streamer->pos = 0-16; //Normal kodda 16 yok ama bizde 16 byte sonrasını okudupu için -16 koydum
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos){
    stream->pos = pos; //Normal kodda 16 yok ama bizde 16 bit sonrasını okuduğu için -16 koydum
    return 0;
}

int diskstreamer_seek2(struct disk_stream* stream, int pos){
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total)
{
    int sector = stream->pos / HESOS_DISK_SECTOR_SIZE;
    int offset = stream->pos % HESOS_DISK_SECTOR_SIZE;
    char buf[HESOS_DISK_SECTOR_SIZE];

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0)
    {
        goto out;
    }

    int total_to_read = total > HESOS_DISK_SECTOR_SIZE ? HESOS_DISK_SECTOR_SIZE : total;
    for (int i = 0; i < total_to_read; i++)
    {
        *(char*)out++ = buf[offset+i];
    }

    // Adjust the stream
    stream->pos += total_to_read;
    if (total > HESOS_DISK_SECTOR_SIZE)
    {
        res = diskstreamer_read(stream, out, total-HESOS_DISK_SECTOR_SIZE);
    }
out:
    return res;
}

void diskstreamer_close(struct disk_stream* stream){
    kfree(stream);
}