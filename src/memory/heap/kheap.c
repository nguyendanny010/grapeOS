#include "kheap.h"
#include "heap.h"
#include "../../config.h"
#include "../../kernel.h"
#include "../memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table; 

/*
 * Initialize the kernel heap.
 */
void kheap_init(){

    int total_table_entries = GRAPEOS_HEAP_SIZE_BYTES / GRAPEOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY *)(GRAPEOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void * end = (void *)(GRAPEOS_HEAP_ADDRESS + GRAPEOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void *)(GRAPEOS_HEAP_ADDRESS), end, &kernel_heap_table);

    if(res< 0) print("Failed to create heap\n");
}

/*
 * Allocates memory within the kernel.
 * @param size the size to allocate.
 */
void * kmalloc(size_t size){
    return heap_malloc(&kernel_heap, size);
}

void * kzalloc(size_t size){
    void * ptr = kmalloc(size);
    if(!ptr) return 0;
    memset(ptr, 0x00, size);
    return ptr;
}
void kfree(void * ptr){
    heap_free(&kernel_heap, ptr);
}
