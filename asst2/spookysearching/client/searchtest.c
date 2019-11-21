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

#include "header.h"
#include "multitest.h"

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    lsobject_t *lso = NULL;
    lsvector_t *lsv = NULL;
    
    int32_t i = 0;

    {
        lso = malloc(sizeof *lso);
        assert(lso);
    }

    {
        lsv = malloc(sizeof *lsv);
        assert(lsv);

        lsv->base = NULL;
        lsv->capacity = 0;
        lsv->subcapacity = 0;

        lso->vec = lsv;
    }

    {
        int32_t key = ARR_SEARCH_VALUE;
        
        {
            lso->search.value = -1;
            lso->search.range_start = 0;
            lso->search.range_end = 0;
            lso->search.partition = 0;
            lso->search.position = -1;
        }

        lso->key = key;
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

            capacity = 250;
            subcapacity = 25;
        } while (capacity % subcapacity != 0);

        assert(subcapacity < capacity);

        lso->vec->capacity = capacity;
        lso->vec->subcapacity = subcapacity;
    }

    {
        int32_t *base = NULL;

        base = calloc(lso->vec->capacity, sizeof *lso->vec->base);
        assert(base);

        lso->vec->base = base;
    }

    {
        for (i = 0; i < lso->vec->capacity; i++) {
            lso->vec->base[i] = i;
        }
    }
    
    {
        int32_t r0 = 0;
        int32_t r1 = 0;
        int32_t temp = 0;

        for (i = 0; i < lso->vec->capacity - 1; i++) {
            r0 = randrnge(0, lso->vec->capacity);
            r1 = randrnge(0, lso->vec->capacity);

            while (r0 == r1) {
                r0 = randrnge(0, lso->vec->capacity);
            }

            temp = lso->vec->base[r0];

            lso->vec->base[r0] = lso->vec->base[r1];
            lso->vec->base[r1] = temp;
        }
    }

    printf("searching for key %d\n", lso->key);

    lsearch(&lso);
    
    if (lso->search.value > -1) {
        printf("\nat partition %d:\n", lso->search.partition);
        printf("found %d at index %d\n\n", lso->key, lso->search.value);
    } else {
        printf("\nsearch failed\n");
    }

    printf("vec size:\t\t%d\n", lso->vec->capacity);
    printf("vec partition size:\t%d\n", lso->vec->subcapacity);
    printf("vec partition count:\t%d\n\n", lso->vec->capacity / lso->vec->subcapacity);

    {
        {
            lso->search.partition = 0;
            lso->search.position = -1;
            lso->search.range_end = 0;
            lso->search.range_start = 0;
            lso->search.value = -1;
        }

        lso->key = 0;

        {
            lso->vec->subcapacity = 0;
            lso->vec->capacity = 0;

            free(lso->vec->base);
            lso->vec->base = NULL;

            free(lso->vec);
            lso->vec = NULL;
        }

        free(lso);
        lso = NULL;
    }

    return EXIT_SUCCESS;
}
