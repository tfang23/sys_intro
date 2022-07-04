/* File: explicit.c
 * Author: Tiantian Fang
 *
 * This file contains my implementation of the explicit allocator.
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

struct ListedBl
{
    struct ListedBl *prev;
    struct ListedBl *next;
};

static struct ListedBl *first_listed_bl;

/* Function: roundup_bl (from bump.c)
 *
 * Parameters:
 * sz - size to be rounded up
 * mult - rounding multiple
 *
 * Returns: 
 * the rounded size
 *
 * This function rounds up the block size to the given multiple.
 * If the size is smaller than the minimum block size requirement,
 * roundup to the minimum size.
 */
size_t roundup_bl(size_t sz, size_t mult) {
    if (sz <= sizeof(struct ListedBl)) {
        return sizeof(struct ListedBl);
    }
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

/* Function: add_listed_bl
 *
 * Parameters:
 * header - pointer to the header of the listed block to be made
 * pl_size - payload size
 *
 * Returns: 
 * pointer to listed block
 *
 * This function makes a free block and returns a pointer to its listed block
 */
struct ListedBl *add_listed_bl(void *hd, size_t pl_size) {
    *(size_t *)hd = pl_size;

    //add block to the front of the list
    struct ListedBl *cur_bl = plptr_of(hd);
    cur_bl->prev = NULL;
    cur_bl->next = first_listed_bl;
    if (first_listed_bl != NULL) {
        first_listed_bl->prev = cur_bl;
    }
    first_listed_bl = cur_bl;

    return cur_bl;
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
    if (heap_size < ALIGNMENT + sizeof(struct ListedBl)) {
        return false;
    }

    first_hd = heap_start;
    total_size = heap_size;
    first_listed_bl = NULL;
    add_listed_bl(first_hd, total_size - ALIGNMENT);
    return true;
}

/* Function: remove_listed_bl
 *
 * Parameters:
 * cur - pointer to the listed block to be removed
 *
 * This function removes a listed block from the list.
 */
void remove_listed_bl(struct ListedBl *cur) {
    if (cur == first_listed_bl) {
        first_listed_bl = cur->next;
    }
    else {
        if (cur->prev != NULL) {
            cur->prev->next = cur->next;
        }
        if (cur->next != NULL) {
            cur->next->prev = cur->prev;
        }
    }
}

/* Function: resizesmaller
 *
 * Parameters:
 * cur - current listed block
 * pl_size - current payload size
 * needed_size - needed size
 *
 * Returns: 
 * pointer to the payload of the block
 *
 * This function resizes a block to fit the needed size most tightly possible.
 */
void *resizesmaller(struct ListedBl *cur, size_t pl_size, size_t needed_size) {
    void *cur_hd = hdptr_of(cur);
    
    if (isfree(cur_hd)) {
        remove_listed_bl(cur);
    }
    //see if we can fit another free block
    if (pl_size - needed_size >= ALIGNMENT + sizeof(struct ListedBl)) {
        add_listed_bl((char *)cur + needed_size, pl_size - needed_size - ALIGNMENT);
        *(size_t *)cur_hd = needed_size;
    }

    *(size_t *)cur_hd = (*(size_t *)cur_hd) | 1;
    return cur;
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

    struct ListedBl *cur_bl = first_listed_bl;
    void *cur_hd;
    size_t pl_size;
    
    while (cur_bl != NULL) {
        
        cur_hd = hdptr_of(cur_bl);
        pl_size = get_pl_size(cur_hd);
        
        if (pl_size >= needed_size) {            
            return resizesmaller(cur_bl, pl_size, needed_size);           
        }
        cur_bl = cur_bl->next;
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
    size_t needed_size = roundup_bl(requested_size, ALIGNMENT);
    return firstfit(needed_size);
}

/* Function: coalescefree
 *
 * Parameters:
 * cur_hd - current header
 * next_hd - next header
 *
 *
 * This function coalesces 2 neighboring free blocks. 
 */
void coalescefree(void *cur_hd, void *next_hd) {
    *(size_t *)cur_hd = *(size_t *)cur_hd + ALIGNMENT + *(size_t *)next_hd;
    remove_listed_bl((struct ListedBl *)((char *)next_hd + ALIGNMENT));
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
        void *cur_hd = (char *)ptr - ALIGNMENT;
        
        if (!isfree(cur_hd)) {
            add_listed_bl(cur_hd, get_pl_size(cur_hd));
            void *next_hd = get_next_hdptr(cur_hd);
            
            if (isfree(next_hd)) {
                coalescefree(cur_hd, next_hd);
            }
        }
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
    if (old_ptr == NULL) {
        return mymalloc(new_size);
    }
    else if (new_size == 0) {
        myfree(old_ptr);
        return NULL;
    }

    size_t needed_size = roundup_bl(new_size, ALIGNMENT);
    void *old_hd = hdptr_of(old_ptr);
    size_t old_size = get_pl_size(old_hd);
    void *cur_hd = (char *)old_ptr + old_size;
    //if we can fit in the original block, resize it smaller
    if (needed_size <= old_size) {
        return resizesmaller(old_ptr, old_size, needed_size);
    }
    //see if we can find and coalesce free blocks to the right
    else if (((char *)cur_hd < ((char *)first_hd + total_size))
             && isfree(cur_hd)) {
        void *next_hd = get_next_hdptr(cur_hd);
            
        while (((char *)next_hd < ((char *)first_hd + total_size))
               && isfree(next_hd)) {
            coalescefree(cur_hd, next_hd);
            next_hd = get_next_hdptr(cur_hd);
        }
        size_t combined_size = old_size + ALIGNMENT + get_pl_size(cur_hd);
        if (needed_size <= combined_size) {
            remove_listed_bl(plptr_of(cur_hd));
            *(size_t *)old_hd = combined_size | 1;
            return resizesmaller(old_ptr, combined_size, needed_size);
        }
    }
    //if nothing works out, malloc to another place
    void *new_ptr = mymalloc(new_size);
    if (new_ptr != NULL) {
        memcpy(new_ptr, old_ptr, old_size);
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
    void *cur_hd = first_hd;
    size_t pl_used = 0;
    size_t pl_free = 0;
    size_t nused = 0;
    size_t nfree = 0;
    
    while ((char *)cur_hd < (char *)first_hd + total_size) {        
        if (!isfree(cur_hd)) {
            pl_used += get_pl_size(cur_hd);
            nused ++;
        }
        else {
            pl_free += get_pl_size(cur_hd);
            nfree ++;
            struct ListedBl *cur_bl = first_listed_bl;
            int count = 0;
            while (cur_bl != NULL) {
                if (cur_hd == (char *)cur_bl - ALIGNMENT) {
                    count ++;
                }
                cur_bl = cur_bl->next;
            }
            if (count == 0) {
                printf("Free block at address %p is not in the free list.\n", cur_hd);
                breakpoint();
                return false;
            }
            if (count > 1) {
                printf("Free block at address %p is listed more than once in the free list.\n", cur_hd);
                breakpoint();
                return false;
            }
            
        }

        cur_hd = get_next_hdptr(cur_hd);   
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
    
    struct ListedBl *cur_bl = first_listed_bl;
    int list_length = 0;
    while (cur_bl != NULL) {
        list_length ++;
        cur_hd = hdptr_of(cur_bl);
        if (!isfree(cur_hd)) {
            printf("Not all blocks in the free list are marked as free.\n");
            breakpoint();
            return false;
        }    
        cur_bl = cur_bl->next;
    }
    if (list_length != nfree) {
        printf("Length of the free list doesn't match the count of free blocks.\n");
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
    printf("The explicit list of free blocks is below:\n");
    struct ListedBl *cur_bl = first_listed_bl;
    while (cur_bl != NULL) {
        printf("\n%p", cur_bl);
        cur_bl = cur_bl->next;
    }
}
