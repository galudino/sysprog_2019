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
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    int instance = 0;

    size_t *arr = NULL;
    size_t capacity = 0;
    size_t subcap = 0;

    size_t search_val = 28;

    size_t i = 0;
    long int index = 0;
    size_t temp = 0;
    
    srand(time(NULL));

    do {
        ++i;

        capacity = randrnge(ARR_RANGE_START, ARR_RANGE_END + 1);
        subcap = ARR_SUBCAP;
        
        if (subcap >= capacity) {
            continue;
        }

        printf("capacity: %lu\nsubcap: %lu\n\n", capacity, subcap);
    } while (capacity % subcap != 0 && subcap < capacity);

    printf("did %lu times\n", i);

    printf("CAPACITY: %lu\nSUBCAP: %lu\n\n", capacity, subcap);

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

    return EXIT_SUCCESS;
}
