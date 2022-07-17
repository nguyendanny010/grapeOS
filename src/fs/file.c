#include "file.h"
#include "../config.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../status.h"

struct filesystem* filesystems[GRAPEOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[GRAPEOS_MAX_FILE_DESCRIPTORS];

/**
 * @brief 
 * Returns an empty filesystem
 * @return struct filesystem** 
 */
static struct filesystem** fs_get_free_filesystem(){
    int i = 0;
    for(i = 0; i < GRAPEOS_MAX_FILESYSTEMS; i++){
        if(filesystems[i] == 0){
            return &filesystems[i];
        }
    }
    return 0;
}

/**
 * @brief 
 * Sets the address of the free filesystem to point to the passed in filesystem
 * @param filesystem 
 */
void fs_insert_filesystem(struct filesystem* filesystem){
    struct filesystem** fs;
    fs = fs_get_free_filesystem();
    if(!fs){
        print("Problem inserting filesystem");
        while(1){}
    }
    *fs = filesystem;
}

static void fs_static_load(){
    //fs_insert_filesystem(fat16_init());
}

/**
 * @brief 
 * Loads all the available file system
 */
void fs_load(){
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

/**
 * @brief 
 * Initializes the filesystem
 */
void fs_init(){
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

/**
 * @brief 
 * 
 * Finds a free file descriptor to use
 * @param desc_out 
 * @return int 0 on success. -ENOMEM: 4 on failure
 */
static int file_new_descriptor(struct file_descriptor** desc_out){
    int res = -ENOMEM;
    for(int i = 0; i < GRAPEOS_MAX_FILE_DESCRIPTORS; i++){

        // Find a free file descriptor to use
        if(file_descriptors[i] == 0){
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

/**
 * @brief 
 * Returns the corresponding file descriptor based on index
 * @param fd File descriptor index
 * @return struct file_descriptor* 
 */
static struct file_descriptor* file_get_descriptro(int fd){
    if(fd <= 0 || fd >= GRAPEOS_MAX_FILE_DESCRIPTORS){
        return 0;
    }

    // Descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];

}

/**
 * @brief 
 * Returns a resolved filesystem 
 * @param disk 
 * @return struct filesystem* 
 */
struct filesystem* fs_resolve(struct disk* disk){
    struct filesystem* fs = 0;
    for(int i = 0; i < GRAPEOS_MAX_FILESYSTEMS; i++){
        if(filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0){
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

int fopen(const char* filename, const char* mode){
    return -EIO;

}