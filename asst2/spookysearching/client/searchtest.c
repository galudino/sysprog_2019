/**
 *  @file       searchtest.c
 *  @brief      Client source file for Asst2: Spooky Search
 *
 *  @author     Gemuele Aludino
 *  @date       04 Nov 2019
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

#include <unistd.h>
#include <time.h>
#include <math.h>

#include "multitest.h"

/**
 *  Determine the memory "block" distance between two addresses,
 *  beg and end, by their "block" widths, determined by sizeof(TYPENAME)
 *
 *  @param[in]  beg     the beginning address
 *  @param[in]  end     the ending address
 *  @param[in]  width   sizeof(TYPENAME), or size in bytes of TYPENAME
 *
 *  @return     distance between beg and end
 *
 *  Precondition: beg and end must be non-NULL and pointing to memory
 *                of the same data type
 */
ptrdiff_t ptr_distance(const void *beg, const void *end, size_t width) {
    char *start = (((char *)(beg)) + (width));
    char *finish = (((char *)(end)) + (width));

    return ((finish - start) / width);
}

void function() {
    int instance = 0;

    size_t *arr = NULL;
    size_t capacity = 0;
    size_t subcap = 0;
    size_t parts = 0;

    size_t search_val = 28;

    size_t i = 0;
    long int index = 0;
    size_t temp = 0;
    
    srand(time(NULL));

    do {
        ++i;

        capacity = randrnge(ARR_RANGE_START, ARR_RANGE_END);
        subcap = randrnge(ARR_RANGE_START_SUB, ARR_RANGE_END_SUB);

        printf("capacity: %lu\nsubcap: %lu\n\n", capacity, subcap);
    } while (capacity % subcap != 0);

    parts = capacity / subcap;

    printf("Took %lu iterations to find a (capacity %% subcap) that != 0.\n", i);

    printf("CAPACITY (capacity of array): %lu\nSUBCAP (subcapacity of array): %lu\nPARTS (amount of procs/threads): %lu\n\n", capacity, subcap, parts);

    arr = calloc(capacity, sizeof *arr);

    if (arr == NULL) {
        fprintf(stderr, 
               "calloc failed to allocate %lu bytes.\n", 
                capacity * sizeof *arr);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < capacity; i++) {
        arr[i] = i;
    }

    for (i = 0; i < capacity; i++) {
        size_t r0 = randrnge(0, capacity);
        size_t r1 = randrnge(0, capacity);

        while (r0 == r1) {
            r0 = randrnge(0, capacity);
        }

        temp = arr[r0];
            
        arr[r0] = arr[r1];
        arr[r1] = temp;
    }
  
    for (i = 0; i < capacity; i++) {
        if (arr[i] == search_val) {
            index = i;
            printf("found %lu at %lu\n", search_val, i);
            break;
        }
    }

    for (instance = 1; instance < TEST_NUM; instance++) {
        size_t r0 = randrnge(0, capacity);

        temp = arr[r0];
        
        arr[r0] = arr[index];
        arr[index] = temp;

        index = -1;

        for (i = 0; i < capacity; i++) {
            if (arr[i] == search_val) {
                index = i;
                printf("[%d]: found %lu at %lu\n", instance, search_val, i);
                break;
            }
        }
    }
    
    free(arr);
    arr = NULL;
}

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    lsargs_t *lsa = NULL;
    size_t i = 0;

    {
        lsa = malloc(sizeof *lsa);
        assert(lsa);
    }

    {
        size_t key = ARR_SEARCH_VALUE;

        lsa->array.base = NULL;
        lsa->array.capacity = 0;
        lsa->array.subcapacity = 0;
    
        lsa->search.key = key;
        lsa->search.value = -1;
        lsa->search.range_start = 0;
        lsa->search.range_end = 0;
        lsa->search.partition = 0;
    }

    srand(time(NULL));

    {
        size_t capacity = 0;
        size_t subcapacity = 0;
        i = 0;

        do {
            ++i;

            capacity = randrnge(ARR_RANGE_START, ARR_RANGE_END);
            subcapacity = randrnge(ARR_RANGE_START, ARR_RANGE_END);
        } while (capacity % subcapacity != 0);

        lsa->array.capacity = capacity;
        lsa->array.subcapacity = subcapacity;
    }

    {
        size_t *base = NULL;

        base = calloc(lsa->array.capacity, sizeof *lsa->array.base);
        assert(base);

        lsa->array.base = base;
    }

    {
        for (i = 0; i < lsa->array.capacity; i++) {
            lsa->array.base[i] = i;
        }
    }

    {
        size_t r0 = 0;
        size_t r1 = 0;
        size_t temp = 0;

        for (i = 0; i < lsa->array.capacity; i++) {
            r0 = randrnge(0, lsa->array.capacity);
            r1 = randrnge(0, lsa->array.capacity);

            while (r0 == r1) {
                r0 = randrnge(0, lsa->array.capacity);
            }

            temp = lsa->array.base[r0];

            lsa->array.base[r0] = lsa->array.base[r1];
            lsa->array.base[r1] = temp;
        }
    }
    
    for (i = 0; i < lsa->array.capacity; i++) {
        bool partition_boundary_reached 
        = ((i + 1) % lsa->array.subcapacity == 0);

        if (partition_boundary_reached) {
            lsa->search.range_start = ((i + 1) - lsa->array.subcapacity);
            lsa->search.range_end = lsa->search.range_start + lsa->array.subcapacity;

            lsearch(&lsa);

            ++lsa->search.partition;

            if (lsa->search.value > -1) {
                break;
            }
        }
    }

    if (lsa->search.value > -1) {
        printf("\nat partition %lu:\n", lsa->search.partition);
        printf("found %lu at index %ld\n\n", lsa->search.key, lsa->search.value);
    } else {
        printf("search failed\n");
    }

    printf("array size: %lu\n", lsa->array.capacity);
    printf("array partition size: %lu\n", lsa->array.subcapacity);
    printf("array partition count: %lu\n\n", lsa->array.capacity / lsa->array.subcapacity);

    {
        lsa->search.partition = 0;
        lsa->search.range_end = 0;
        lsa->search.range_start = 0;
        lsa->search.value = -1;
        lsa->search.key = 0;

        lsa->array.subcapacity = 0;
        lsa->array.capacity = 0;

        free(lsa->array.base);
        lsa->array.base = NULL;

        free(lsa);
        lsa = NULL;
    }

    return EXIT_SUCCESS;
}
