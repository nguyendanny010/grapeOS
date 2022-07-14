#include "heap.h"
#include "../../kernel.h"
#include "../../status.h"
#include "../memory.h"
#include <stdbool.h>

/*
 * Checks if the table's total number of blocks is correct given end and ptr.
 */
static int heap_validate_table(void * ptr, void * end, struct heap_table* table){
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / GRAPEOS_HEAP_BLOCK_SIZE;
    if(table->total != total_blocks){
        res = -EINVARG;
        goto out;
    }

out:
    return res;
}

static bool heap_validate_alignment(void * ptr){
    return ((unsigned int)ptr % GRAPEOS_HEAP_BLOCK_SIZE) == 0;
}

/*
 * Creates the heap.
 */
int heap_create(struct heap * heap, void * ptr, void * end, struct heap_table * table){
    int res = 0;
    if(!heap_validate_alignment(ptr) || !heap_validate_alignment(end)){
        res = -EINVARG;
        goto out;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);
    if(res < 0) goto out;

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
    return res;
}

/*
 * Aligns the value to the upper block.
 */
static uint32_t heap_align_value_to_upper(uint32_t val){
    if(val % GRAPEOS_HEAP_BLOCK_SIZE == 0) return val;

    val = (val - (val % GRAPEOS_HEAP_BLOCK_SIZE));
    val += GRAPEOS_HEAP_BLOCK_SIZE;
    return val;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry){
    return entry & 0x0f;
}

/*
 * Returns the starting block in the heap where total_blocks are free
 * @param *heap the current heap.
 * @param total_blocks the total amount of blocks which must be available.
 */
int heap_get_start_block(struct heap * heap, uint32_t total_blocks){
    struct heap_table * table = heap->table;
    int current_block = 0;
    int starting_block = -1;

    for(size_t i = 0; i < table->total; i++){
        // If this entry isn't free
        if(heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE){
            current_block = 0;
            starting_block = -1;
            continue;
        }
        // If this is the first block
        if(starting_block == -1) starting_block = i;
        current_block++;
        // We have enough space
        if(current_block == total_blocks) break;
    }

    if(starting_block == -1) return -ENOMEM;

    return starting_block;
}

/*
 * Returns the address of the corresponding block
 * @param *heap the current heap.
 * @param block the block whose corresponding address is returned.
 */
void * heap_block_to_address(struct heap * heap, int block){
    return heap->saddr + (block * GRAPEOS_HEAP_BLOCK_SIZE);
}

/*
 * Marks total_blocks starting from start_block as taken within the heap.
 * @param *heap the current heap.
 * @param start_block the block to start from.
 * @param total_blocks total amount of blocks to mark as taken.
 */
void heap_mark_blocks_taken(struct heap * heap, int start_block, int total_blocks){
    int end_block = (start_block + total_blocks) - 1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN || HEAP_BLOCK_IS_FIRST;

    if(total_blocks > 1) entry |= HEAP_BLOCK_HAS_NEXT;

    for(int i = start_block; i <= end_block; i++){
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if(i != end_block - 1) entry |= HEAP_BLOCK_HAS_NEXT;
    }
}

/*
 * Retrieves the starting block of total_blocks then marks the blocks starting
 * from that address as taken. 
 * @param *heap the current heap.
 * @param total_blocks total amount of blocks in memory to allocate.
 */
void * heap_malloc_blocks(struct heap * heap, uint32_t total_blocks){
    void * address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);

    if(start_block < 0) goto out;

    address = heap_block_to_address(heap, start_block);

    // Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);

out:
    return address;
}


/*
 * Marks blocks from starting_block as free.
 * @param *heap the current heap.
 * @param starting_block the block to start from.
 */
void heap_mark_blocks_free(struct heap * heap, int starting_block){
    struct heap_table * table = heap->table;

    for(int i = starting_block; i < (int)table->total; i++){
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        
        if(!(entry & HEAP_BLOCK_HAS_NEXT)) break;
    }
}

/*
 * Returns the corresponding block number of the address
 * @param *heap the current heap.
 * @param *address the address whose block to return.
 */
int heap_address_to_block(struct heap * heap, void * address){
    return ((int)(address - heap->saddr)) / GRAPEOS_HEAP_BLOCK_SIZE;
}

/*
 * Marks size amount of blocks as taken within the heap.
 * @param *heap the address of the heap.
 * @param size block size.
 */
void * heap_malloc(struct heap * heap, size_t size){
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / GRAPEOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

/*
 * Frees the designated ptr address from the heap.
 * @param *heap the address of the heap where ptr should be freed from.
 * @param ptr the address to be freed.
 */
void heap_free(struct heap * heap,void * ptr){
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}