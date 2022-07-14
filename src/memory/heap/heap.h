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

/*
 * Checks if the table's total number of blocks is correct given end and ptr.
 */
static int heap_validate_table(void * ptr, void * end, struct heap_table* table);

/*
 * Creates the heap.
 */
int heap_create(struct heap * heap, void * ptr, void * end, struct heap_table * table);

/*
 * Aligns the value to the upper block.
 */
static uint32_t heap_align_value_to_upper(uint32_t val){

/*
 * Returns the starting block in the heap where total_blocks are free
 * @param *heap the current heap.
 * @param total_blocks the total amount of blocks which must be available.
 */
int heap_get_start_block(struct heap * heap, uint32_t total_blocks);

void * heap_malloc_blocks(struct heap * heap, uint32_t total_blocks);

/*
 * Returns the address of the corresponding block
 * @param *heap the current heap.
 * @param block the block whose corresponding address is returned.
 */
void * heap_block_to_address(struct heap * heap, int block);

/*
 * Marks total_blocks starting from start_block as taken within the heap.
 * @param *heap the current heap.
 * @param start_block the block to start from.
 * @param total_blocks total amount of blocks to mark as taken.
 */
void heap_mark_blocks_taken(struct heap * heap, int start_block, int total_blocks);

/*
 * Retrieves the starting block of total_blocks then marks the blocks starting
 * from that address as taken. 
 * @param *heap the current heap.
 * @param total_blocks total amount of blocks in memory to allocate.
 */
void * heap_malloc_blocks(struct heap * heap, uint32_t total_blocks);

/*
 * Marks blocks from starting_block as free.
 * @param *heap the current heap.
 * @param starting_block the block to start from.
 */
void heap_mark_blocks_free(struct heap * heap, int starting_block);

/*
 * Returns the corresponding block number of the address
 * @param *heap the current heap.
 * @param *address the address whose block to return.
 */
int heap_address_to_block(struct heap * heap, void * address);

/*
 * Marks size amount of blocks as taken within the heap.
 * @param *heap the address of the heap.
 * @param size block size.
 */
void * heap_malloc(struct heap * heap, size_t size);

/*
 * Frees the designated ptr address from the heap.
 * @param *heap the address of the heap where ptr should be freed from.
 * @param ptr the address to be freed.
 */
void heap_free(struct heap * heap, void * ptr);

#endif