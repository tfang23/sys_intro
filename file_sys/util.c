/* FILE: util.c
 * Tiantian Fang CS107
 *
 * This file contains the function binsert.
 */
#include "samples/prototypes.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/* FUNCTION: binsert
 *
 * Paramters:
 *    key - the key to be found
 *    base - the initial member pointed to
 *    p_nelem - number of members
 *    width - size of each member
 * Returns: NA
 * 
 * This function performs a binary search for the key and if no matching element is found, it inserts the key into the proper position in the sorted array.
 */
void *binsert(const void *key, void *base, size_t *p_nelem, size_t width,
              int (*compar)(const void *, const void *)) {
    void *p = base;
    void *old_base = base;
    for(size_t nremain = *p_nelem; nremain != 0; nremain >>= 1) {
        p = (char *)base + (nremain >> 1) * width;
        int sign = compar(key, p);
        if (sign == 0) {
            return p;
        }
        if (sign > 0) {
            base = (char *)p + width;
            p = base;
            nremain--;
        }
    }
    size_t size = (char *)p - (char *)old_base;
    //makes room for the new member
    memmove((char *)p + width, p, (*p_nelem) * width - size);
    //copies in the new member
    memcpy(p, key, width);
    (*p_nelem)++;
    return p;
}
