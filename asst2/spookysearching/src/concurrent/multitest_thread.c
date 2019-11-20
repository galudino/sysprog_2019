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

static void *handler_lsearch(void *arg);

void lsobject_search(lsobject_t **l) {
    lsobject_t *lso = *(lsobject_t **)(l);

    int32_t i = 0;

    int32_t range_start = 0;
    int32_t range_end = 0;
    int32_t partition = -1;

    for (i = 0; i < lso->vec.capacity; i += lso->vec.subcapacity) {
        range_start = i;
        range_end = i + lso->vec.subcapacity;

        ++partition;

        lso->search->value = -1;
        lso->search->range_start = range_start;
        lso->search->range_end = range_end;
        lso->search->partition = partition;
        lso->search->position = -1;       
            
        handler_lsearch(l);

        if (lso->search->value > -1) {
            break;
        }
    }

    printf("position: %d\nindex: %d\npartition number: %d\n\n", lso->search->position, lso->search->value, lso->search->partition);
}

static void *handler_lsearch(void *arg) {
    lsobject_t *lso = *(lsobject_t **)(arg);

    int32_t j = 0;
    int32_t position = 0;

    /*
    printf("\n\nBeginning search for partition %d...\n", lso->search->partition);
    */

    for (j = lso->search->range_start; j < lso->search->range_end; j++) {
        if (lso->vec.base[j] == lso->key) {
            lso->search->value = j;

            printf("partition %d, vec[%d]:\t%d\nhandler: found key %d at index %d, partition %d\n\n", lso->search->partition, j, lso->vec.base[j], lso->key, lso->search->value, lso->search->partition);

            break;
        }

        ++position;
    }

    lso->search->position = lso->search->value != -1 ? position : lso->vec.subcapacity;

    /*
    printf("\nSearch ended for partition %d.\nwas %s\n\n", lso->search->partition, lso->search->value != -1 ? "SUCCESSFUL" : "unsuccessful");
    */

    return arg;
}

void *func(void *arg) {
    int32_t *n = (int *)(arg);
    int32_t *tid = malloc(sizeof *tid);
    (*tid) = 1008 + (*n);

    printf("did thread %d\n", (*n));

    pthread_exit(tid);
}

void test() {
    pthread_t threads[TEST_NUM];

    int32_t i = 0;
    int32_t status = -1;
    int32_t *tid = NULL;
    int32_t thread_args[TEST_NUM];

    /* deploy all threads */
    for (i = 0; i < TEST_NUM; i++) {
        thread_args[i] = i;

        printf("spawning thread %d\n", i);
        status = pthread_create(threads + i, NULL, func, thread_args + i);
    }

    /* join all threads, free resources */
    for (i = 0; i < TEST_NUM; i++) {
        pthread_join(threads[i], (void **)(&tid));
        
        printf("status of %d: %s (%d)\n\n", (*tid), strerror(status), status);

        free(tid);
        tid = NULL;
    }
}
