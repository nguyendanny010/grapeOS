#include "disk.h"
#include "../io/io.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../config.h"

struct disk disk;
/**
 * @brief 
 * Reads one sector from the disk
 * 
 * @param lba       The logical block address to read from
 * @param total     The total number of blocks to read from lba
 * @param buffer    A pointer to the buffer we load that data to
 * @return 0        On success 
 */
int disk_read_sector(int lba, int total, void* buffer){
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* ptr = (unsigned short*) buffer;
    for(int b = 0; b < total; b++){
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08)){
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for(int i = 0; i < 256; i++){
            *ptr = insw(0x1f0);
            ptr++;
        }
    }
    return 0;
}

/**
 * @brief 
 * Responsible for searching for disks and initializng them
 */
void disk_search_and_init(){
    memset(&disk, 0, sizeof(disk));
    disk.type = GRAPEOS_DISK_TYPE_REAL;
    disk.sector_size = GRAPEOS_SECTOR_SIZE;
}

/**
 * @brief 
 * Gets the disk based on index
 * @param index If index isn't 0 then it must be an invalid disk
 * @return struct disk* 
 */
struct disk* disk_get(int index){
    if(index != 0)
        return 0;

    return &disk;
}

/**
 * @brief 
 * 
 * @param idisk 
 * @param lba 
 * @param total 
 * @param buf 
 * @return int 
 */
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf){
    if(idisk != &disk){
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}