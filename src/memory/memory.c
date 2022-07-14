#include "memory.h"

/*
 * Sets the values at the memory address ptr to c.
 */
void * memset(void * ptr, int c, size_t size){
    char * c_ptr = (char *) ptr;
    for(int i = 0; i < size; i++){
        c_ptr[i] = (char) c;
    }
    return ptr;
}