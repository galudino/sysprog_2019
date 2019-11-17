/**
 *  @file       multitest_proc.c
 *  @brief      Process-specialized source file for Asst2: Spooky Search
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

void lsargs_search(lsargs_t **l) {
    lsargs_t *lsa = *(lsargs_t **)(l);
    int i = 0;
    pid_t pid = -1;
    int status = -1;

    for (i = 0; i < lsa->array.capacity; i++) {
        bool partition_boundary_reached 
        = ((i + 1) % lsa->array.subcapacity == 0);

        if (partition_boundary_reached) {
            ++lsa->search.partition;

            lsa->search.range_start = ((i + 1) - lsa->array.subcapacity);
            lsa->search.range_end = lsa->search.range_start + lsa->array.subcapacity;

            pid = fork();

            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                handler_lsearch(&lsa);

                if (lsa->search.value > -1) {
                    exit(lsa->search.partition);
                }

                exit(EXIT_SUCCESS);
            } else {
                wait(&status);

                if (status > 0) {     
                    lsa->search.partition = status / lsa->array.subcapacity;
                    lsa->search.value = i;
                    break;
                }
            }
        }
    }
}


/*
void lsargs_search(lsargs_t **l) {
    lsargs_t *lsa = *(lsargs_t **)(l);
    size_t i = 0;

    for (i = 0; i < lsa->array.capacity; i++) {
        bool partition_boundary_reached 
        = ((i + 1) % lsa->array.subcapacity == 0);

        if (partition_boundary_reached) {
            ++lsa->search.partition;

            lsa->search.range_start = ((i + 1) - lsa->array.subcapacity);
            lsa->search.range_end = lsa->search.range_start + lsa->array.subcapacity;

            handler_lsearch(&lsa);

            if (lsa->search.value > -1) {
                break;
            }
        }
    }
}
*/

void *handler_lsearch(void *arg) {
    lsargs_t *lsa = *(lsargs_t **)(arg);
    int32_t i = 0;
    int32_t position = 0;

    for (i = lsa->search.range_start; i < lsa->search.range_end; i++) {
        printf("array[%d]: %d\n", i, lsa->array.base[i]);

        if (lsa->array.base[i] == lsa->search.key) {
            lsa->search.value = i;
            lsa->search.position = position;
            break;
        }

        ++position;
    }

    return NULL;
}

void *func(void *arg) {
    int *n = (int *)(arg);

    printf("did process %d\n", (*n));

    return NULL;
}

void test() {
    int i = -1;
    int status = -1;

    pid_t pid = 0;

    while (i < TEST_NUM) {
        /* do whatever you have to do here prior to spawning procs */

        /* from here on out, each child proc will do its own work */
        pid = fork();

        if (pid == 0) {
            /* do work here, call a function, do something... */
            func(&i);

            /* exit when done */
            exit(EXIT_SUCCESS);
        } else {
            wait(&status);
            printf("status of %d: %s (%d)\n\n", pid, strerror(status), status);
            ++i;
        }
    }
}
