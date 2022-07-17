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

/**
 * @brief 
 * Compares the first count bytes of memory address s1 and memory address s2
 * @param s1 The first memory address
 * @param s2 The second memory address
 * @param count The number of bytes to compare
 * @return int 0 on equality. -1 on s1 lesser. 1 on s2 lesser.
 */
int memcmp(void* s1, void* s2, int count){
    char* c1 = s1;
    char* c2 = s2;
    while(count-- > 0){
        if(*c1++ != *c2++){
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }
    return 0;
}