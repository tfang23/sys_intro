/* File: implicit.c
 * Author: Tiantian Fang
 *
 * This file contains my implementation of the implicit allocator.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "allocator.h"
#include "debug_break.h"

static void *first_hd;
static size_t total_size; 

/* Function: roundup (from bump.c)
 *
 * Parameters:
 * sz - size to be rounded up
 * mult - rounding multiple
 *
 * Returns: 
 * the rounded size
 *
 * This function rounds up the size to the given multiple.
 */
size_t roundup(size_t sz, size_t mult) {
    return (sz + mult - 1) & ~(mult - 1);
}

/* Function: hdptr_of
 *
 * Parameters:
 * plptr - pointer to the payload
 *
 * Returns: 
 * pointer to the header
 *
 * This function returns a pointer to the header of given payload.
 */
void *hdptr_of(void *plptr) {
    return (char *)plptr - ALIGNMENT;
}
/* Function: plptr_of
 *
 * Parameters:
 * hdptr - pointer to the header
 *
 * Returns: 
 * pointer to the payload
 *
 * This function returns a pointer to the payload of given header.
 */
void *plptr_of(void *hdptr) {
    return (char *)hdptr + ALIGNMENT;
}

/* Function: isfree
 *
 * Parameters:
 * hdptr - pointer to the header of a block
 *
 * Returns: 
 * whether the block is free
 *
 * This function returns whether the given block is free.
 */
bool isfree(void *hdptr){
    return ((*(size_t *)hdptr) & 1) == 0;
}

/* Function: get_pl_size
 *
 * Parameters:
 * hdptr - pointer to the header of a block
 *
 * Returns: 
 * the block's payload size
 *
 * This function returns the block's payload size.
 */
size_t get_pl_size(void *hdptr) {
    if (isfree(hdptr)) {
        return *(size_t *)hdptr;
    }
    else {       
        return *(size_t *)hdptr - 1;
    }
}

/* Function: get_next_hdptr
 *
 * Parameters:
 * cur_hdptr - pointer to the current header
 *
 * Returns: 
 * pointer to the next header
 *
 * This function returns a pointer to the next header in the heap.
 */
void *get_next_hdptr(void *cur_hdptr) {
    return (char *)cur_hdptr + ALIGNMENT + get_pl_size(cur_hdptr);
}

/* Function: make_block
 *
 * Parameters:
 * hdptr - pointer to the header of the block to be made
 * pl_size - payload size
 * free - if the block should be free
 *
 * Returns: 
 * pointer to the payload
 *
 * This function makes a block and returns a pointer to its payload. 
 */
void *make_block(void *hdptr, size_t pl_size, bool free) {
    if (!free) {
        pl_size |= 1;
    }
    *(size_t *)hdptr = pl_size;
    return plptr_of(hdptr);
}

/* Function: myinit
 *
 * Parameters:
 * heap_start - pointer to the start of heap
 * heap_size - size of heap
 *
 * Returns: 
 * if the initialization was successful
 *
 * This function is called before making any allocation
 * requests. It returns true if initialization was 
 * successful, or false otherwise. The myinit function can be 
 * called to reset the heap to an empty state. When running 
 * against a set of of test scripts, the test harness calls 
 * myinit before starting each new script.
 */
bool myinit(void *heap_start, size_t heap_size) {
    //The heap needs to have a size of at least 2 * ALIGNMENT
    if (heap_size < 2 * ALIGNMENT) {
        return false;
    }
    else {
        first_hd = heap_start;
        total_size = heap_size;
        make_block(heap_start, heap_size - ALIGNMENT, true);
        return true;
    }
}

/* Function: firstfit
 *
 * Parameters:
 * needed_size - needed size to be allocated
 *
 * Returns: 
 * pointer to the payload of the block that the needed size can fit in
 *
 * This function finds a free block that can accommodate the needed size 
 * using first fit and then returns a pointer to its payload.
 */
void *firstfit(size_t needed_size) {
    size_t cur_pl_size;    
    void *cur_hd = first_hd;
    
    while ((char *)cur_hd < (char *)first_hd + total_size) {
        cur_pl_size = get_pl_size(cur_hd);
   
        if (isfree(cur_hd) && (cur_pl_size >= needed_size)) {
            //check if we have room for another free block
            if (cur_pl_size >= needed_size + 2 * ALIGNMENT) {
                make_block((char *)cur_hd + ALIGNMENT + needed_size,
                           cur_pl_size - needed_size - ALIGNMENT, true);
                return make_block(cur_hd, needed_size, false);
            }
            else {
                return make_block(cur_hd, cur_pl_size, false);
            }
        }
        cur_hd = get_next_hdptr(cur_hd);
    }
    return NULL;
}

/* Function: mymalloc
 *
 * Parameters:
 * requested_size - requested size to be allocated
 *
 * Returns: 
 * pointer to the payload of the block that the requested size can fit in
 *
 * This function alllocates the requested size 
 * and then returns a pointer to its payload.
 */
void *mymalloc(size_t requested_size) {
    if (requested_size == 0 || requested_size > MAX_REQUEST_SIZE) {
        return NULL;
    }
    size_t needed_size = roundup(requested_size, ALIGNMENT);
    return firstfit(needed_size);
}

/* Function: myfree
 *
 * Parameters:
 * ptr - pointer to the payload to be freed
 *
 * This function frees a previously allocated block.
 */
void myfree(void *ptr) {
    if (ptr != NULL) {
        void *hd = hdptr_of(ptr);
        *(size_t *)hd -= 1;
    }
}

/* Function: myrealloc
 *
 * Parameters:
 * old_ptr - pointer to the payload to be reallocated
 * new_size - the new size requested
 *
 * This function reallocates a previously allocated block.
 */
void *myrealloc(void *old_ptr, size_t new_size) {
    if  (old_ptr == NULL) {
        return mymalloc(new_size);
    }
    if (new_size == 0) {
        myfree(old_ptr);
        return NULL;
    }
 
    void *new_ptr = mymalloc(new_size);
    if (new_ptr != NULL) {
        memcpy(new_ptr, old_ptr, new_size);
        myfree(old_ptr);
    }
    return new_ptr;
}

/* Function: validate_heap
 *
 * Return true if all is ok, or false otherwise.
 * This function is called periodically by the test
 * harness to check the state of the heap allocator.
 */
bool validate_heap() {
    void *hd = first_hd;
    size_t pl_used = 0;
    size_t pl_free = 0;
    size_t nused = 0;
    size_t nfree = 0;
    
    while ((char *)hd < (char *)first_hd + total_size) {
        size_t cur_pl_size = get_pl_size(hd);
        if (cur_pl_size < ALIGNMENT) {
            printf("Block at address %p has incorrect payload.\n", hd);
            breakpoint();
            return false;
        }
        if (!isfree(hd)) {
            pl_used += cur_pl_size;
            nused ++;
        }
        else {
            pl_free += cur_pl_size;
            nfree ++;
        }
        hd = get_next_hdptr(hd);   
    }
    
    if (pl_used + nused * ALIGNMENT > total_size) {
        printf("Used more heap than available.\n");
        breakpoint();
        return false;
    }    
    if (pl_used + pl_free + (nused + nfree) * ALIGNMENT != total_size) {
        printf("Sum of all block sizes doesn't match total size of the heap.\n");
        breakpoint();
        return false;
    }   
    return true;
}

/* Function: dump_heap
 *
 * This function traverses the heap and prints out information about each block.
 */
void dump_heap() {
    printf("Heap segment starts at address %p, ends at %p.",
           first_hd, (char *)first_hd + total_size);
    void *cur = first_hd;
    while ((char *)cur < (char *)first_hd + total_size) {
        printf("\n%p: ", cur);
        printf("%lu ", *(size_t *)cur);
    }
}
