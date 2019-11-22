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
    int32_t *base = NULL;
    int32_t capacity = 0;
    int32_t subcapacity = 0;

    int32_t key = -1;
    int32_t result = -1;

    int32_t i = 0;

    srand(time(NULL));

    {
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
    }

    {
        base = calloc(capacity, sizeof *base);
        assert(base);
    }

    {
        for (i = 0; i < capacity; i++) {
            base[i] = i;
        }
    }

    {
        int32_t r0 = 0;
        int32_t r1 = 0;
        int32_t temp = 0;

        for (i = 0; i < capacity - 1; i++) {
            r0 = randrnge(0, capacity);
            r1 = randrnge(0, capacity);

            while (r0 == r1) {
                r0 = randrnge(0, capacity);
            }

            temp = base[r0];

            base[r0] = base[r1];
            base[r1] = temp;
        }
    }

    key = ARR_SEARCH_VALUE;

    printf("searching for key %d\n", key);
    result = lsearch_int32(base, capacity, subcapacity, key);

    if (result <= -1) {
        printf("search failed\n");
    } else {
        printf("result: %d\n", result);
    }

    free(base);
    base = NULL;

    return EXIT_SUCCESS;
}
