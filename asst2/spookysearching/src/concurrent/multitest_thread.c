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

void *func(void *arg) {
    int *n = (int *)(arg);
    int *tid = malloc(sizeof *tid);
    (*tid) = 1008 + (*n);

    printf("did thread %d\n", (*n));

    pthread_exit(tid);
}

void test() {
    pthread_t threads[10];

    int i = 0;
    int status = -1;

    while (i < 10) {
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

