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

#define elapsed_time_ns(BEF, AFT)                                              \
    (((double)((pow(10.0, 9.0) * AFT.tv_sec) + (AFT.tv_nsec))) -               \
     ((double)((pow(10.0, 9.0) * BEF.tv_sec) + (BEF.tv_nsec))))

#define elapsed_time_ms(BEF, AFT)                                              \
    ((elapsed_time_ns(BEF, AFT) * pow(10.0, -6.0)))
#define elapsed_time_s(BEF, AFT) (elapsed_time_ns(BEF, AFT) * pow(10.0, -9.0))

#define convert_ns_to_s(NS) ((NS) * pow(10.0, -9.0))
#define convert_ns_to_ms(NS) ((NS) * pow(10.0, -6.0))
#define convert_ns_to_mcs(NS) ((NS) * (pow(10.0, -3.0)))

#define MCS "µs"

#define randrnge(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))

void test();

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
    size_t capacity = randrnge(256, 65536);

    size_t search_val = 28;

    size_t i = 0;
    size_t index = 0;
    size_t temp = 0;
    
    srand(time(NULL));

    arr = malloc(sizeof *arr * capacity);

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

    for (instance = 1; instance < 50; instance++) {
        size_t r0 = randrnge(0, capacity);

        temp = arr[r0];
        
        arr[r0] = arr[index];
        arr[index] = temp;

        for (i = 0; i < capacity; i++) {
            if (arr[i] == search_val) {
                index = i;
                printf("found %lu at %lu\n", search_val, i);
                break;
            }
        }
    }

    free(arr);
    arr = NULL;

    return EXIT_SUCCESS;
}
