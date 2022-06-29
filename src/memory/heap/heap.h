#ifndef HEAP_H
#define HEAP_H
#include "../../config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b100000000
#define HEAP_BLOCK_IS_FIRST  0b010000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table{
    HEAP_BLOCK_TABLE_ENTRY * entries;
    size_t total;
};

struct heap{
    struct heap_table * table;
    // Start address of the heap data pool
    void * saddr;
};

int heap_create(struct heap * heap, void * ptr, void * end, struct heap_table * table);
int heap_get_start_block(struct heap * heap, uint32_t total_blocks);
void * heap_block_to_address(struct heap * heap, int block);
void * heap_malloc_blocks(struct heap * heap, uint32_t total_blocks);
void heap_mark_blocks_taken(struct heap * heap, int start_block, int total_blocks);
void * heap_malloc_blocks(struct heap * heap, uint32_t total_blocks);
void * heap_malloc(struct heap * heap, size_t size);
void heap_free(struct heap * heap, void * ptr);

#endif