#include "heap.h"
#include "../../kernel.h"
#include "../../status.h"
#include "../memory.h"
#include <stdbool.h>

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

void * heap_malloc(size_t size){
    return 0;
}

void heap_free(void * ptr){
    return 0;
}