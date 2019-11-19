/**
 *  @file       multitest_thread.c
 *  @brief      Thread-specialized source file for Asst2: Spooky Search
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

#include "multitest.h"

void *handler_lsearch(void *arg) {
    lsargs_t *lsa = (lsargs_t *)(arg);
    int32_t j = 0;
    int32_t position = 0;

    printf("\n");
    printf("\nBeginning search for partition %d...\n", lsa->search.partition);
    for (j = lsa->search.range_start; j < lsa->search.range_end; j++) {
        printf("partition %d, array[%d]:\t%d\n", lsa->search.partition, j, lsa->array.base[j]);

        if (lsa->array.base[j] == lsa->search.key) {
            lsa->search.value = j;
            lsa->search.position = position;

            printf("\n\nhandler: found %d at %d\n\n", lsa->search.key, lsa->search.value);

            break;
        }

        ++position;
    }

    printf("\nSearch ended for partition %d.\n", lsa->search.partition);
    
    printf("was %s\n", lsa->search.value != -1 ? "SUCCESSFUL" : "unsuccessful");
    
    lsa->search.position 
    = lsa->search.value != -1 ? position : -6;
    
    printf("\n");

    return NULL;
}

void lsargs_search(lsargs_t *l) {
    lsargs_t *lsa = (lsargs_t *)(l);

    int i = 0;

    for (i = 0; i < lsa->array.capacity; i += lsa->array.subcapacity) {
        ++lsa->search.partition;

        lsa->search.range_start = i;
        lsa->search.range_end = lsa->search.range_start +
lsa->array.subcapacity;

        handler_lsearch(&lsa);

        if (lsa->search.value > -1) {
            break;
        }
    }
}

void *func(void *arg) {
    int *n = (int *)(arg);
    int *tid = malloc(sizeof *tid);
    (*tid) = 1008 + (*n);

    printf("did thread %d\n", (*n));

    pthread_exit(tid);
}

void test() {
    pthread_t threads[TEST_NUM];

    int i = 0;
    int status = -1;

    while (i < TEST_NUM) {
        status = pthread_create(threads + i, NULL, func, &i);

        if (status) {

        } else {
            int *tid = NULL;
            pthread_join(threads[i], (void **)(&tid));
            printf("status of %x: %s (%d)\n\n", *tid, strerror(status), status);
            free(tid);
            tid = NULL;
            ++i;
        }
    }
}
