/**
 *  @file       mymalloc.c
 *  @brief      Source file for Asst1: ++malloc
 *
 *  @author     Gemuele Aludino
 *  @date       30 Sep 2019
 *  @copyright  Copyright © 2019 Gemuele Aludino
 */
/**
 *  Copyright © 2019 Gemuele Aludino
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *  THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mymalloc.h"

char myblock[MYMALLOC__BLOCK_SIZE];







/**
 *  @brief      Allocates size bytes from myblock
 *              and returns a pointer to the allocated memory.
 * 
 *  Precondition: size parameter provided is within (0, 4096]
 *  
 *  @param[in]  size        desired memory by user (in bytes)
 *  @param[in]  filename    for use with __FILE__ directive
 *  @param[in]  lineno      for use with __LINE__ directive
 * 
 *  @return     on success, a pointer to a block of memory of size size.
 *              on failure, NULL
 */
/*
void *mymalloc(size_t size, const char *filename, size_t lineno) {
    void *block = NULL;
    block = sbrk(size);
    return block == ((void *)(-1)) ? NULL : block;
}
*/
void *mymalloc(size_t size, const char *filename, size_t lineno) {
    static char *blockpos = myblock;
    static int capacity = MYMALLOC__BLOCK_SIZE;
    void *block = NULL;
    int sz = (int)(size);

    if ((capacity - size) >= 0) {
        block = blockpos;
        blockpos += size;
        capacity -= sz;
        printf("capacity: %d\n", capacity);
    }

    if (capacity < 0) {
        printf("Out of memory!\n");
    }
    
    return block;
}

/**
 *  @brief      Frees the space pointer to by ptr, which must have been
 *              returns by a previous call to mymalloc.
 *  
 *  @param[out]  ptr         address of the memory to free
 *  @param[in]   filename    for use with the __FILE__ directive
 *  @param[in]   lineno      for use with the __LINE__ directive 
 */
void myfree(void *ptr, const char *filename, size_t lineno) {

}

