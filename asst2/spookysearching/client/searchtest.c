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
    int32_t i = 0;

    {
        lsa = malloc(sizeof *lsa);
        assert(lsa);
    }

    {
        int32_t key = ARR_SEARCH_VALUE;

        lsa->array.base = NULL;
        lsa->array.capacity = 0;
        lsa->array.subcapacity = 0;
    
        lsa->search.key = key;
        lsa->search.value = -1;
        lsa->search.range_start = 0;
        lsa->search.range_end = 0;
        lsa->search.partition = -1;
        lsa->search.position = -1;
    }

    srand(time(NULL));

    {
        int32_t capacity = 0;
        int32_t subcapacity = 0;
        i = 0;

        do {
            ++i;

            /*
            capacity = randrnge(ARR_RANGE_START, ARR_RANGE_END);
            subcapacity = randrnge(ARR_RANGE_START_SUB, ARR_RANGE_END_SUB);
            */
            capacity = 1000;
            subcapacity = 250;
        } while (capacity % subcapacity != 0);

        assert(lsa->search.key < capacity);

        lsa->array.capacity = capacity;
        lsa->array.subcapacity = subcapacity;
    }

    {
        int32_t *base = NULL;

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
        int32_t r0 = 0;
        int32_t r1 = 0;
        int32_t temp = 0;

        for (i = 0; i < lsa->array.capacity - 1; i++) {
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

    printf("searching for key %d\n", lsa->search.key);

    lsargs_search(lsa);
    
    if (lsa->search.value > -1) {
        printf("\nat partition %d:\n", lsa->search.partition);
        printf("found %d at index %d\n\n", lsa->search.key, lsa->search.value);
    } else {
        printf("\nsearch failed\n");
    }

    printf("array size:\t\t%d\n", lsa->array.capacity);
    printf("array partition size:\t%d\n", lsa->array.subcapacity);
    printf("array partition count:\t%d\n\n", lsa->array.capacity / lsa->array.subcapacity);

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
