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
 *  the rights to use, copy, modify, block_merge, publish, distribute, sublicense,
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

typedef struct header header_t;
struct header {
    uint32_t size;
    bool free;

    header_t *next;
};

#define MYMALLOC__END_ADDR ((void *)(myblock + (MYMALLOC__BLOCK_SIZE)))

/**< myblock: block of memory in ./data/BSS segment */
static char myblock[MYMALLOC__BLOCK_SIZE];

/**< header_t: freelist - base pointer to myblock, initial header */
static header_t *freelist = (header_t *)(myblock);

/**< header_t: pos - position/cursor pointer */
static header_t *pos = NULL;

/**< header_t: private functions - init/split/merge/validate */
static void header_init_list();
static void header_split_block(header_t *curr, size_t size);
static void header_merge_block(header_t *curr);
static bool header_validator(void *ptr);

static uint32_t block_used = 0;
static uint32_t block_free = 0;
static uint32_t merge_counter = 0;

/**
 *  Experimental - may or may not be used.
 *  rbheader_t is 24 bytes (left-leaning red-black tree)
 *  header_t is 16 bytes (singly-linked list)
 */

typedef unsigned char rbt_color;
#define RBT_BLACK   false
#define RBT_RED     true

typedef struct rbheader rbheader_t;
struct rbheader {
    uint32_t size;
    bool free;

    rbt_color color;
    rbheader_t *left;
    rbheader_t *right;
};

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
void *mymalloc(size_t size, const char *filename, size_t lineno) {
    header_t *curr = NULL;
    header_t *prev = NULL;

    void *result = NULL;

    /**
     *  If mymalloc has not been called yet,
     *  initialize the free list by creating a header (node)
     *  within myblock, and giving the header its starting values.
     */
    if (freelist->size == 0) {
        header_init_list();
    }

    /**
     *  Cursor variable curr is set to the base address of freelist,
     *  which is set to the base address of myblock.
     */
    curr = freelist;

    /**
     *  First sanity check: is the size request greater than 0?
     *  If not, do not continue -- return NULL.
     */
    if (size == 0) {
        ulog(stderr, "[ERROR]", filename, __func__, lineno, 
        "Allocation input value must be a nonzero integer of positive magnitude.");
        return NULL;
    }

    /**
     *  We traverse the free list (myblock) and search for 
     *  a header that is associated with an unused block of memory.
     *
     *  We reject headers denoting occupied blocks,
     *  and headers representing blocks of sizes less than what we are
     *  looking for.
     */
    while (curr->size < size || curr->free == false || curr->next) {
        prev = curr;
        curr = curr->next;

        /**
         *  If the header that was just visited is representing a free block,
         *  and the current header is also representing a free block,
         *  perform a coalescence between them.
         */
        if (prev->free && curr->free) {
            header_merge_block(prev);
        }
    }
    
    if (curr->size == size) {
        /**
         *  If we have found a header representing a block
         *  that is an exact fit for the allocation request,
         *  mark it as 'in use', and prepare to return its address
         *  to the user.
         *
         *  curr is the address of the header, but we do not
         *  want to return the header, but rather, we want
         *  to return the memory that the header represents --
         *  the base address of the desired block begins
         *  after the entirety of curr's block.
         *
         *  curr + 1
         *  - or -
         *  (char *)(curr) + (sizeof(header_t))
         *
         *  yields the desired address.
         *  Basically, we want to advance sizeof(header_t) bytes
         *  past the address curr, so we can get to the memory
         *  that curr represents.
         */
        curr->free = false;
        result = curr + 1;
    } else if ((curr->size) > (size + sizeof *curr)) {
        /**
         *  If curr is greater than the size requested
         *  (plus sizeof(header_t)), we can split curr's memory --
         *  curr's size will become that of the request quantity,
         *  and the other partition will have the remaining free storage.
         */    
        header_split_block(curr, size);
        
        curr->free = false;
        result = curr + 1;
    } else {
        ulog(stderr, "[ERROR]", filename, __func__, lineno, 
        "Unable to allocate %lu bytes.", size);
    }

    block_used += result ? 1 : 0;
    return result;
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
    header_t *header = NULL;
    bool block_in_range = false;

    /**
     *  Sanity check: is ptr a NULL pointer, an already-freed pointer,
     *  or a pointer unrelated to mymalloc/myfree?
     *
     *  If validator comes back true, continue -- otherwise,
     *  return from this function.
     */
    if (header_validator(ptr) == false) {
        return;
    }

    /**
     *  Since we have made it this far,
     *  we can safely treat ptr as a (header_t *).
     *
     *  Note at this point, header is still
     *  the allocated memory that the user wants to release,
     *  it is NOT the header the precedes this memory --
     *  not yet, anyway.
     */
    header = (header_t *)(ptr);

    /**
     *  Is the address of ptr within range of the base address of
     *  myblock, and myblock's last byte address?
     */
    block_in_range 
    = ptr > (void *)(myblock) && (ptr <= MYMALLOC__END_ADDR);

    /**
     *  By decrementing header, we now have access to the
     *  header that represents the memory addressed by ptr.
     *
     *  (we are working in reverse order of mymalloc, in a sense)
     */
    --header;

    if (header->free) {
        /**
         *  If the header reports that this block of memory
         *  is already free, there is nothing left to do but
         *  report the findings to the user -- return afterward.
         */
        ulog(stderr, "[ERROR]", filename, __func__, lineno, 
        "Cannot release memory for inactive storage -- did you already call free on this address?");
        return;
    } else {
        if (block_in_range) {
            header_t *prev = NULL;
            /**
             *  If the memory block represented by header
             *  indeed belongs to myblock
             *  (it is in range of possible addresses),
             *  set the free flag to true.
             *  (we don't have to zero-out anything --
             *   the memory will be overwritten over time.)
             */
            header->free = true;

            --block_used;
            ++block_free;

            /**
             *  We can use this opportunity to coalesce blocks --
             *  if the adjacent block is reported to be free, 
             *  merge it with the block associated with header.
             */
            if (header->next->free) {
                header_merge_block(header);
            }
            
            /**
             *  Now will traverse the entire free list
             *  and search for more blocks that can be coalesced.
             *
             *  (This shouldn't run all the time...
             *   figure out a way to have this run some of time,
             *   maybe a threshold of free blocks needs to be met
             *   or do a full merge every x mallocs and/or frees)
             */
            header = freelist;
            while (header->next) {
                prev = header;
                header = header->next;

                if (prev->free && header->free) {
                    prev->size += header->size + sizeof *header;
                    prev->next = prev->next->next;
                    --block_free;
                }
            }
        } else {
            /**
             *  If the pointer provided has no connection
             *  to mymalloc/myfree, notify the user and return.
             */
            ulog(stderr, "[ERROR]", filename, __func__, lineno, 
            "This pointer does not refer to a valid allocation by mymalloc.");
        }
    }
}

/**
 *  @brief  Output the current state of myblock to a FILE stream dest
 *
 *  @param[in]  dest        a FILE * stream, stdout, stderr, or a file
 *  @param[in]  filename    for use with the __FILE__ macro
 *  @param[in]  funcname    for use with the __func__ macro
 *  @param[in]  lineno      for use with the __LINE__ macro
 */
void header_fputs(FILE *dest, 
                  const char *filename, 
                  const char *funcname,
                  size_t lineno) {
    header_t *header = freelist;

    uint32_t free_space = 0;
    uint32_t used_space = 0;

    uint32_t total_data_in_use = 0;
    uint32_t block_no_metadata = 0;

    if (header->size == 0) {
        fprintf(dest, "------------------------------------------\n");
        fprintf(dest, "No allocations have been made yet.\n\n");
        fprintf(dest, "[%s:%lu] %s%s%s\n%s%s %s%s\n", 
        filename, lineno, KCYN, funcname, KNRM, KGRY, __DATE__, __TIME__, KNRM);
        fprintf(dest, "------------------------------------------\n\n");
        return;
    }

    fprintf(dest, "------------------------------------------\n");
    fprintf(dest, "%sBlock Address%s\t%sStatus%s\t\t%sBlock Size%s\n", 
    KWHT_b, KNRM, KWHT_b, KNRM, KWHT_b, KNRM);
    fprintf(dest, "-------------\t------\t\t----------\n");

    while (header) {
        const char *free = header->free ? KGRN"free"KNRM : KRED_b"in use"KNRM;
        free_space += header->free ? header->size : 0;
        used_space += header->free ? 0 : header->size;
        fprintf(dest, "%p\t%s\t\t%lu\n", (header + 1), free, header->size);

        header = header->next;
    }

    total_data_in_use = used_space + (sizeof *header * block_used);

    block_no_metadata = 
    MYMALLOC__BLOCK_SIZE - (sizeof *header * (block_free + block_used));

    fprintf(dest, "------------------------------------------\n");
    fprintf(dest, "Used blocks in list:\t%lu\n", block_used);
    fprintf(dest, "Free blocks in list:\t%lu\n\n", block_free);

    fprintf(dest, "Free space:\t\t%lu of %lu bytes\n", 
    free_space, MYMALLOC__BLOCK_SIZE);

    fprintf(dest, "Available for client:\t%lu of %lu bytes\n\n", 
    free_space, block_no_metadata);

    fprintf(dest, "Total data in use:\t%lu of %lu bytes\n", 
    total_data_in_use, MYMALLOC__BLOCK_SIZE);

    fprintf(dest, "Client data in use:\t%lu of %lu bytes\n\n", 
    used_space, block_no_metadata);

    fprintf(dest, "[%s:%lu] %s%s%s\n%s%s %s%s\n", 
    filename, lineno, KCYN, funcname, KNRM, KGRY, __DATE__, __TIME__, KNRM);
    fprintf(dest, "------------------------------------------\n\n");
}

/**
 *  @brief  Initializes myblock by assigning it its first header node
 *          with its starting values
 */
static void header_init_list() {
    /**
     *  A newly initialized freelist will have one
     *  header/node, with its allotted capacity - sizeof(header_t).
     *  Of course, this header represents a free block of memory,
     *  and it has no other header to link with. 
     */
    freelist->size = (MYMALLOC__BLOCK_SIZE - sizeof *freelist);
    freelist->free = true;
    freelist->next = NULL;

    ++block_free;
}

/**
 *  @brief  Creates a new block by partitioning the memory referred to
 *          by curr into size bytes -- the remaining memory
 *          becomes part of the new block created in this function
 *
 *  @param[out] curr    header to the memory that will be split
 *  @param[in]  size            desired size for curr
 */
static void header_split_block(header_t *curr, size_t size) {
    /**
     *  We want to treat curr as a (char *) --
     *  the address of a one-byte figure.
     *
     *  That way, when we add any integral values to it,
     *  pointer arithmetic becomes standard arithmetic, and we
     *  can fine-tune the exact amount of bytes that we want to
     *  advance from address curr.
     *
     *  We want to find an address for new_header, and we will
     *  start from curr.
     *
     *  We will advance sizeof(header_t), or (sizeof *curr) bytes
     *  from curr, plus the intended size for the block to be split.
     *
     *  (char *)(curr) + (sizeof(header_t) + size)
     *
     *  But, we cannot simply assign this to new_header, because
     *  it has been type-coerced to be a (char *).
     *  So, we cast the entirety of the expression to (header_t *),
     *  the intended type.
     *
     *  (header_t *)((char *)(curr) + (sizeof(header_t) + size))
     */
    header_t *new_header = (header_t *)((char *)(curr) + (sizeof *curr + size));

    /**
     *  Now that new_header has been given its new home,
     *  its values can be assigned.
     *
     *  The size of new_header will be
     *      curr->size (curr's size, soon to be former size)
     *          minus
     *      the requested size (what curr's size will become, shortly)
     *          minus
     *      sizeof(header_t), or sizeof *new_header
     *
     *  Remember, the header takes up room too, and that is why we must
     *  subtract sizeof(header_t) (or sizeof *new_header) from the overall
     *  quantity.
     *
     *  new_header is a free block, and it will have its next pointer
     *  assigned to whatever curr's next pointer was.
     */
    new_header->size = curr->size - size - sizeof *new_header;
    new_header->free = true;
    new_header->next = curr->next ? curr->next : NULL;

    /**
     *  curr will now take on its new size value,
     *  and its next pointer will be addressed to new_header.
     */
    curr->size = size;
    curr->next = new_header;
}

/**
 *  @brief  Coalesces memory referred to by curr and curr->next
 *          into one block
 *
 *  @param[out] curr    pointer to header_t, refers to memory within myblock
 * 
 *  Precondition: curr->free && curr->next->free, else undefined behavior
 *
 *  This function is called by mymalloc or myfree, when the precondition
 *  above is fulfilled. It is not to be called unless conditions for
 *  coalescence are clearly defined.
 */
static void header_merge_block(header_t *curr) {    
    --block_free;
    curr->size += curr->next->size + sizeof *curr->next;

    if (curr->next != NULL) {
        curr->next = curr->next->next;
    }

    /**
     *  If there are only two blocks left,
     *  and they are both free --
     *  coalesce them both.
     *
     *  ** may not work ? **
     */
    /*
    if (freelist->free && freelist->next->free) {
        --block_free;
        freelist->size += freelist->next->size + sizeof *freelist->next;
        freelist->next = NULL;
    }
    */
}

/**
 *  @brief  Determines if ptr is NULL, 
 *          or if nonnull, determines if it is a pointer allocated by mymalloc
 *
 *  @param[in]  ptr     The pointer to verify
 *
 *  @return     true if a valid pointer (header can be recovered),
 *              false otherwise
 */
static bool header_validator(void *ptr) {
    header_t *header = NULL;
    bool result = false;

    if (ptr == NULL) {
        ulog(stderr, "[ERROR]", __FILE__,  __func__, __LINE__, 
        "Called myfree on a NULL pointer.");
    } else {
        header = (header_t *)(ptr);
        --header;

        result = header->size > 0 
        && header->size <= (MYMALLOC__BLOCK_SIZE - sizeof *header);
    }

    return result;
}
