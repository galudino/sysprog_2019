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
 *  the rights to use, copy, modify, block_merge, publish, distribute,
 *  sublicense, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
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
#include "utils.h"

/**
 *  @typedef    header_t
 *  @brief      Alias for (struct header)
 *
 *  All instances (struct header) will be addressed as header_t.
 */
typedef struct header header_t;

/**
 *  @struct     header
 *  @brief      Represents metadata preceding a block of memory that is
 *              dynamically allocated by mymalloc
 */
struct header {
    int16_t size; /**< size of block after header_t, negative size means used */
};

/**< myblock: block of memory in ./data/BSS segment */
static char myblock[MYMALLOC__BLOCK_SIZE];

#define MYMALLOC__END_ADDR ((void *)(myblock + (MYMALLOC__BLOCK_SIZE) - 1))

#define MYMALLOC__END_BLOCK                                                    \
    (header_t *)((myblock + (MYMALLOC__BLOCK_SIZE)) - sizeof(header_t))

static uint16_t merge_counter = 0;

/**< header_t: pos - position/cursor pointer */
static header_t *pos = NULL;

/**< header_t: initializer */
static void header_init_list();

/**< header_t: split/merge/global coalesce */
static void header_split_block(header_t *curr, size_t size);
static void header_merge_block(header_t *curr);
static void header_coalesce(header_t *curr);

/**< header_t: utilities */
static bool header_validator(void *ptr);

#define header_size(HEADER) (abs(HEADER->size))
#define header_size_split(HEADER, SIZE)                                        \
    ((int16_t)((HEADER->size) - (SIZE) - (sizeof(header_t))))

#define header_is_free(HEADER) ((HEADER->size) >= (0))
#define header_is_used(HEADER) ((HEADER->size) < (0))

#define header_toggle(HEADER) ((HEADER->size) *= (-1))

#define header_next(HEADER)                                                    \
    ((header_t *)((char *)(HEADER) + (sizeof(header_t)) + (header_size(HEADER))))

#define header_is_last(HEADER)                                                 \
    ((header_t *)((char *)((header_next(HEADER))) - (sizeof(header_t))) == (MYMALLOC__END_BLOCK))

/**
 *  @brief      Allocates size bytes from myblock
 *              and returns a pointer to the allocated memory.
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
    header_t *next = NULL;

    /**
     *  If mymalloc has not been called yet,
     *  initialize the free list by creating a header
     *  within myblock, and giving the header its starting value(s).
     */
    if (((header_t *)(myblock))->size == 0) {
        header_init_list();
    }

    /**
     *  First sanity check: is the size request within [1, 4095)
     *  If not, do not continue -- return NULL.
     */
    if (size == 0 || size > (MYMALLOC__BLOCK_SIZE - sizeof(header_t))) {
        ulog(stderr,
             "[ERROR]",
             filename,
             "mymalloc",
             lineno,
             "Allocation input "
             "value must be within [1, %lu) bytes.\tAttempted "
             "allocation: %lu bytes",
             (MYMALLOC__BLOCK_SIZE - sizeof(header_t)) + 1,
             size);
        return NULL;
    }

    /**
     *  Cursor variable next is set to the base address of myblock.
     */
    curr = (header_t *)(myblock);

    /**
     *  We traverse the free list (myblock) and search for
     *  a header that is associated with an unused block of memory.
     *
     *  We reject headers denoting occupied blocks,
     *  and headers representing blocks of sizes less than what we are
     *  looking for.
     *
     *  curr becomes NULL when there are no more blocks to traverse.
     */
    do {
        /**
         *  Retrieve the "lookahead" header from position curr.
         */
        next = header_is_last(curr) ? NULL : header_next(curr);

        /**
         *  If curr represents a free block...
         */
        if (header_is_free(curr)) {
            /**
             *  If curr's right adjacent header (if applicable)
             *  represents a free block, we can merge them together.
             *  This should help to reduce fragmentation in the long run.
             */
            if (next && header_is_free(next)) {
                header_merge_block(curr);
            }

            /**
             *  If the block represented by curr is greater than or equal
             *  to the requested size, we can leave the loop.
             */
            if (header_size(curr) >= (size)) {
                break;
            }
        }

        curr = next;
    } while (next != NULL);

    /**
     *  If curr is nonnull, we have found what we are looking for.
     */
    if (curr) {
        /**
         *  If the block represented by curr is bigger than
         *  the requested value, size, it will be split,
         *  so that curr ends up representing a block with a count of
         *  size bytes.
         *
         *  However, the split must also result in a second block
         *  with enough space to hold a new header representing a block
         *  of at least size 1.
         *
         *  If the split were to occur such that there was not enough
         *  room for a header with a block of at least size 1,
         *  the block will not be split.
         *
         *  header_size_split(curr, size) expands to:
         *  curr->size - size - sizeof(header_t)
         *
         *  So,
         *      the size of the block represented by curr
         *          minus
         *      the size requested for allocation by the client
         *          minus
         *      the size of (header_t) -- block metadata.
         *
         *  must be greater than or equal to 1
         *  to be worth a split.
         *
         *  Basically, curr->size (the size of a candidate block)
         *  must be at least (requested size + (sizeof(header_t) + 1))
         *  in order to qualify for a split.
         */
        bool eligible_for_split = header_size_split(curr, size) >= 1;

        if (eligible_for_split) {
            header_split_block(curr, size);
        }

        /**
         *  curr will now represent an occupied block.
         */
        header_toggle(curr);
    } else {
        ulog(stderr,
             "[ERROR]",
             filename,
             "my_malloc",
             lineno,
             "Unable to allocate %lu bytes. (header requires at least "
             "%lu bytes)",
             size,
             sizeof *curr);
    }

    /**
     *  If curr is nonnull,
     *  we will return the base address
     *  of the client's requested memory. (skipping over the header/metadata)
     *
     *  (curr + 1) is equivalent to ((char *)(curr) + (sizeof(header_t) * 1)).
     *
     *  We simply move sizeof(header_t) bytes
     *  past the address described by curr and return it.
     */
    return curr ? (curr + 1) : NULL;
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
    header_t *curr = NULL;
    bool block_in_range = false;

    /**
     *  Sanity check: is ptr a
     *      - NULL pointer,
     *      - already-freed pointer,
     *      - or a pointer unrelated to mymalloc/myfree?
     *
     *  If validator comes back true, continue --
     *  otherwise, return from this function.
     */
    if (header_validator(ptr) == false) {
        return;
    }

    /**
     *  Since we have made it this far,
     *  we can safely treat ptr as a (header_t *).
     *
     *  Note at this point, curr is still
     *  the allocated memory that the user wants to release,
     *  it is NOT the header the precedes this memory --
     *  not yet, anyway.
     */
    curr = (header_t *)(ptr);

    /**
     *  Is the address of ptr within range of the base address of
     *  myblock, and myblock's last byte address?
     */
    block_in_range = ptr >= (void *)((char *)(myblock) + sizeof(header_t)) &&
                     (ptr <= MYMALLOC__END_ADDR);

    /**
     *  By decrementing curr, we now have access to the
     *  header that represents the memory addressed by ptr.
     *
     *  (we are working in reverse order of mymalloc, in a sense)
     */
    --curr;

    if (header_is_free(curr)) {
        /**
         *  If curr reports that this block of memory
         *  is already free, there is nothing left to do but
         *  report the findings to the user -- return afterward.
         */
        ulog(stderr,
             "[ERROR]",
             filename,
             "my_free",
             lineno,
             "Cannot release "
             "memory for "
             "inactive storage "
             "-- did you "
             "already call free "
             "on this address?");
        return;
    } else {
        if (block_in_range) {
            header_t *next = NULL;

            /**
             *  curr will now represent an unoccupied block.
             */
            header_toggle(curr);

            /**
             *  We can use this opportunity to coalesce blocks --
             *  if the adjacent block is reported to be free,
             *  it will be merged with curr's block.
             */
            next = header_is_last(curr) ? NULL : header_next(curr);

            if (next && header_is_free(next)) {
                header_merge_block(curr);
            }

            /**
             *  The entirety of myblock will also be searched for
             *  adjacent free blocks to coalesce.
             */
            header_coalesce((header_t *)(myblock));
        } else {
            /**
             *  If the pointer provided has no relationship whatsoever
             *  to mymalloc/myfree, notify the user and return.
             */
            ulog(stderr,
                 "[ERROR]",
                 filename,
                 "my_free",
                 lineno,
                 "This pointer "
                 "does not "
                 "refer to a "
                 "valid "
                 "allocation by "
                 "mymalloc.");
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
void header_fputs(FILE *dest, const char *filename, const char *funcname, size_t lineno) {
    header_t *header = (header_t *)(myblock);

    struct {
        uint16_t block_used;
        uint16_t block_free;

        uint16_t space_used;
        uint16_t space_free;

        uint16_t bytes_in_use;
        uint16_t block_count_available;

        uint16_t largest_block_used;
        uint16_t largest_block_free;
    } info = { 0, 0, 0, 0, 0, 0, 0, 0 };

    if (header->size == 0) {
        fprintf(dest, "------------------------------------------\n");
        fprintf(dest, "No allocations have been made yet.\n\n");
        fprintf(dest, "[%s:%lu] %s%s%s\n%s%s %s%s\n", filename, lineno, KCYN, funcname, KNRM, KGRY, __DATE__, __TIME__, KNRM);
        fprintf(dest, "------------------------------------------\n\n");
        return;
    }

    fprintf(dest, "------------------------------------------\n");
    fprintf(dest, "%sBlock Address%s\t%sStatus%s\t\t%sBlock Size%s\n", KWHT_b, KNRM, KWHT_b, KNRM, KWHT_b, KNRM);
    fprintf(dest, "-------------\t------\t\t----------\n");

    while (header) {
        bool header_free = header_is_free(header);

        const char *free =
            header_free ? KGRN "free" KNRM : KRED_b "in use" KNRM;

        info.block_used += header_free ? 0 : 1;
        info.block_free += header_free ? 1 : 0;

        info.space_used += header_free ? 0 : header_size(header);
        info.space_free += header_free ? header_size(header) : 0;

        info.largest_block_used =
            (info.largest_block_used < header_size(header)) && !header_free ?
                header_size(header) :
                info.largest_block_used;

        info.largest_block_free = (info.largest_block_free < header_size(header) && header_free ?
                                       header_size(header) :
                                       info.largest_block_free);

        fprintf(dest, "%s%p%s\t%s\t\t%d\n", KGRY, (void *)(header + 1), KNRM, free, header_size(header));

        header = header_is_last(header) ? NULL : header_next(header);
    }

    info.bytes_in_use =
        info.space_used + (sizeof *header * (info.block_used + info.block_free));

    info.block_count_available =
        MYMALLOC__BLOCK_SIZE - (sizeof *header * (info.block_free + info.block_used));

    fprintf(dest, "------------------------------------------\n");

    fprintf(dest, "Used blocks in list:\t%s%u%s blocks\n", KWHT_b, info.block_used, KNRM);
    fprintf(dest, "Free blocks in list:\t%s%u%s blocks\n\n", KWHT_b, info.block_free, KNRM);

    fprintf(dest, "Free space:\t\t%s%u%s of %s%u%s bytes\n", KWHT_b, info.space_free, KNRM, KWHT_b, MYMALLOC__BLOCK_SIZE, KNRM);

    fprintf(dest,
            "Available for client:\t%s%u%s of %s%u%s bytes\n\n",
            KWHT_b,
            info.space_free,
            KNRM,
            KWHT_b,
            info.block_count_available,
            KNRM);

    fprintf(dest,
            "Total data in use:\t%s%u%s of %s%u%s bytes\n",
            KWHT_b,
            info.bytes_in_use,
            KNRM,
            KWHT_b,
            MYMALLOC__BLOCK_SIZE,
            KNRM);

    fprintf(dest,
            "Client data in use:\t%s%u%s of %s%u%s bytes\n\n",
            KWHT_b,
            info.space_used,
            KNRM,
            KWHT_b,
            info.block_count_available,
            KNRM);

    fprintf(dest,
            "Largest used block:\t%s%u%s of %s%u%s bytes\n",
            KWHT_b,
            info.largest_block_used,
            KNRM,
            KWHT_b,
            info.block_count_available,
            KNRM);

    fprintf(dest,
            "Largest free block:\t%s%u%s of %s%u%s bytes\n\n",
            KWHT_b,
            info.largest_block_free,
            KNRM,
            KWHT_b,
            info.block_count_available,
            KNRM);

    fprintf(dest, "Size of metadata:\t%s%lu%s bytes\n\n", KWHT_b, sizeof(header_t), KNRM);

    fprintf(dest, "[%s:%lu] %s%s%s\n%s%s %s%s\n", filename, lineno, KCYN, funcname, KNRM, KGRY, __DATE__, __TIME__, KNRM);
    fprintf(dest, "------------------------------------------\n\n");
}

/**
 *  @brief  Initializes myblock by assigning it its first header node
 *          with its starting value(s)
 */
static void header_init_list() {
    /**
     *  A newly initialized myblock will have one
     *  header/node, with its allotted capacity:
     *  (MYMALLOC__BLOCK_SIZE - sizeof(header_t)).
     */
    ((header_t *)(myblock))->size = (MYMALLOC__BLOCK_SIZE - sizeof(header_t));
}

/**
 *  @brief  Creates a new block by partitioning the memory referred to
 *          by next into size bytes -- the remaining memory
 *          becomes part of the new block created in this function
 *
 *  Precondition: header_is_free(curr) && header_is_free(header_next(curr)),
 *                else undefined behavior
 *
 *  @param[out] curr    header to the block that will be split
 *  @param[in]  size    desired size for curr
 */
static void header_split_block(header_t *curr, size_t size) {
    /**
     *  We want to treat curr as a (char *) --
     *  the address of a one-byte figure.
     *
     *  That way, when we add any integral values to it,
     *  pointer arithmetic becomes standard arithmetic, and we
     *  can fine-tune the exact amount of bytes that we want to
     *  advance from address next.
     *
     *  We want to find an address for new_header, and we will
     *  start from next.
     *
     *  We will advance sizeof(header_t), or (sizeof *next) bytes
     *  from next, plus the intended size for the block to be split.
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
     *  Address range/input check
     */
    if (new_header >= MYMALLOC__END_BLOCK || size == 0 ||
        size >= (MYMALLOC__BLOCK_SIZE - sizeof(header_t))) {
        return;
    }

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
     *  The result, new_header, is a free (unoccupied) block.
     */
    new_header->size = (curr->size - size) - sizeof *new_header;

    /**
     *  curr will now take on its new size value.
     */
    curr->size = size;
}

/**
 *  @brief  Coalesces memory referred to by curr and curr->next
 *          into one block
 *
 *  @param[out] curr    pointer to header_t, refers to memory within myblock
 *
 *  Precondition: header_free(curr) && header_free(header_next(curr))
 *
 *  This function is called by mymalloc or myfree, when the precondition
 *  above is fulfilled. It is not to be called unless conditions for
 *  coalescence are clearly defined.
 */
static void header_merge_block(header_t *curr) {
    header_t *next = header_next(curr);
    curr->size += next->size + sizeof *next;
}

/**
 *  @brief  Traverses the myblock buffer by byte increments of
 *          (sizeof *curr + curr->size) and merges adjacent free blocks
 *          into contigious memory for future allocations
 *
 *  @param[out] curr    pointer to header_t, refers to memory within myblock
 *
 *  Precondition: curr != NULL and header_init_list has been called
 */
static void header_coalesce(header_t *curr) {
    header_t *prev = NULL;

    while (curr != NULL) {
        /**
         *  If the header that was just visited is representing a free block,
         *  and the current header is also representing a free block,
         *  perform a coalescence between them.
         */
        if (prev) {
            if (header_is_free(prev) && header_is_free(curr)) {
                header_merge_block(prev);
            }
        }

        /**
         *  If we haven't found what we are looking for yet,
         *  and curr is not the last header, we move on to the next header.
         */
        prev = curr;
        curr = header_is_last(curr) ? NULL : header_next(curr);
    }
}

/**
 *  @brief  Determines if ptr is NULL,
 *          or if nonnull, determines if it is a pointer allocated by mymalloc
 *
 *  @param[in]  ptr     the pointer to verify
 *
 *  @return     true if a valid pointer (header can be recovered),
 *              false otherwise
 */
static bool header_validator(void *ptr) {
    header_t *header = NULL;
    bool result = false;

    if (ptr == NULL) {
        ulog(stderr,
             "[ERROR]",
             __FILE__,
             "header_validator",
             __LINE__,
             "Called myfree "
             "on a NULL "
             "pointer.");
    } else {
        header = (header_t *)(ptr) - 1;

        result = header_size(header) > 0 &&
                 header_size(header) <= (MYMALLOC__BLOCK_SIZE - sizeof *header);
    }

    return result;
}


#ifndef UTILS_H

bool ulog_attrs_disable[] = { false, false, false, false, false, false, false };

/**
 *  Utility function for debugging/error messages
 *
 *  @param[in]      dest        stream destination
 *  @param[in]      level       literals "BUG", "ERROR", "WARNING", or "LOG"
 *  @param[in]      file        macro __FILE__ is to be used here (by client)
 *  @param[in]      func        macro __func__ is to be used here
 *  @param[in]      line        macro __LINE__ is to be used here
 *  @param[in]      fmt         formatting to be used
 *
 *  @return         character count of buffer (from fprintf)
 */
int ulog(FILE *dest,
         const char *level,
         const char *file,
         const char *func,
         long double line,
         const char *fmt,
         ...) {

    char buffer[MAXIMUM_STACK_BUFFER_SIZE];
    char temp[BUFFER_SIZE];

    const char *color = KNRM;
    const char *blink = "";

    bool found = false;
    bool is_integer = false;
    bool is_currency = *file == '$';

    int j = 0;

    if (streql(level, "[BUG]")) {
        color = KYEL_b;
        found = true;
    }

    if (found == false) {
        if (streql(level, "[LOG]")) {
            color = KCYN_b;
            found = true;
        }
    }

    if (found == false) {
        if (streql(level, "[ERROR]")) {
            color = KRED_b;
            blink = KBNK;
            found = true;
        }
    }

    if (found == false) {
        if (streql(level, "[WARNING]")) {
            color = KMAG_b;
            blink = KBNK;
        }
    }

    sprintf(temp, "%Lf", line);

/* char digit = strchr(temp, '.'); */

#if __STD_VERSION__ >= 199901L
    is_integer = line / (long long int)(line) == 1.000000 || line == 0.00000;
#else
    is_integer = line / (long int)(line) == 1.000000 || line == 0.00000;
#endif /* __STDC_VERSION__ >= 199901L */

    is_integer = is_currency ? false : is_integer;

    if (ulog_attrs_disable[DATE] == false) {
        char date[1024];
        sprintf(date, "%s%s%s ", KGRY, __DATE__, KNRM);

        j += sprintf(buffer + j, "%s", date);
    }

    if (ulog_attrs_disable[TIME] == false) {
        char time[1024];
        sprintf(time, "%s%s%s ", KGRY, __TIME__, KNRM);

        j += sprintf(buffer + j, "%s", time);
    }

    if (ulog_attrs_disable[LEVEL] == false) {
        char leveltype[1024];
        sprintf(leveltype, "%s%s%s%s ", blink, color, level, KNRM);

        j += sprintf(buffer + j, "%s", leveltype);
    }

    if (ulog_attrs_disable[FILENAME] == false && ulog_attrs_disable[LINE]) {
        char filename[1024];
        sprintf(filename, "[%s] ", file);

        j += sprintf(buffer + j, "%s", filename);
    } else if (ulog_attrs_disable[FILENAME] && ulog_attrs_disable[LINE] == false) {
        char linenumber[1024];

        if (is_integer) {
#if __STD_VERSION__ >= 199901L
            sprintf(linenumber, "[%lli] ", (long long int)(line));
#else
            sprintf(linenumber, "[%li] ", (long int)(line));
#endif
        } else {
            if (is_currency) {
                sprintf(linenumber, "[%0.2Lf] ", line);
            } else {
                sprintf(linenumber, "[%Lf] ", line);
            }
        }

        j += sprintf(buffer + j, "%s", linenumber);
    } else if (ulog_attrs_disable[FILENAME] == false && ulog_attrs_disable[LINE] == false) {
        char fileline[1024];

        if (is_integer) {
#if __STD_VERSION__ >= 199901L
            sprintf(fileline, "[%s:%lli] ", file, (long long int)(line));
#else
            sprintf(fileline, "[%s:%li] ", file, (long int)(line));
#endif
        } else {
            if (is_currency) {
                sprintf(fileline, "[%s%0.2Lf] ", file, line);
            } else {
                sprintf(fileline, "[%s:%Lf] ", file, line);
            }
        }

        j += sprintf(buffer + j, "%s", fileline);
    }

    if (ulog_attrs_disable[FUNCTION] == false) {
        char function[1024];
        sprintf(function, "%s%s", KCYN, func);

        j += sprintf(buffer + j, "%s", function);
    }

    if (ulog_attrs_disable[FUNCTION] == false && ulog_attrs_disable[MESSAGE] == false) {
        j += sprintf(buffer + j, "%s", " ");
    }

    if (ulog_attrs_disable[MESSAGE] == false) {
        char message[4096];
        va_list args;

        va_start(args, fmt);
        vsprintf(message, fmt, args);
        va_end(args);

        j += sprintf(buffer + j, "%s%s%s", KNRM_b, message, KNRM);
    }

    return fprintf(dest, "%s\n", buffer);
}
#endif
