#include "streamer.h"
#include "../memory/heap/kheap.h"
#include "../config.h"
/**
 * @brief 
 * Creates a new disk stream
 * @param disk_id 
 * @return struct disk_stream* 
 */
struct disk_stream* diskstreamer_new(int disk_id){
    struct disk* disk = disk_get(disk_id);
    if(!disk){
        return 0;
    }
    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->byte_position = 0;
    streamer->disk = disk;

    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int byte_position){
    stream->byte_position = byte_position;
    return 0;
}

/**
 * @brief 
 * Reads total amount of bytes from stream into out. This function
 * allows us to read a size other than the sector size.
 * @param stream 
 * @param out 
 * @param total 
 * @return int 
 */
int diskstreamer_read(struct disk_stream* stream, void* out, int total){

    int sector = stream->byte_position / GRAPEOS_SECTOR_SIZE;
    int offset = stream->byte_position % GRAPEOS_SECTOR_SIZE;
    char buf[GRAPEOS_SECTOR_SIZE];

    int res = disk_read_block(stream->disk, sector, 1, buf);

    if(res < 0){
        goto out;
    }

    int total_to_read = total > GRAPEOS_SECTOR_SIZE ? GRAPEOS_SECTOR_SIZE : total;

    for(int i = 0; i < total_to_read; i++){
        *(char*)out++ = buf[offset+i];
    }

    // Adjust the stream
    stream->byte_position += total_to_read;

    if(total > GRAPEOS_SECTOR_SIZE){
        res = diskstreamer_read(stream, out, total - GRAPEOS_SECTOR_SIZE);
    }

out:
    return res;
}

/**
 * @brief 
 * Closes the disk_stream
 * @param stream 
 */
void diskstreamer_close(struct disk_stream* stream){
    kfree(stream);
}