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

#include <signal.h>

void lsargs_search(lsargs_t **l) {
    lsargs_t *lsa = *(lsargs_t **)(l);

    int i = 0;
    int status = -1;
    pid_t pid = -1;

    struct {
        pid_t *base;
        size_t capacity;
        size_t length;
    } v_pid = { NULL, 16, 0 };

    v_pid.base = calloc(16, sizeof *v_pid.base);
    assert(v_pid.base);
    v_pid.capacity = 16;
    v_pid.length = 0;

    v_pid.base[v_pid.length++] = getpid();

    for (i = 0; i < lsa->array.capacity; i += lsa->array.subcapacity) {
        ++lsa->search.partition;

        lsa->search.range_start = i;
        lsa->search.range_end = i + lsa->array.subcapacity;

        pid = fork();

        if (pid == -1) {
            perror("fork\n");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            handler_lsearch(&lsa);
            printf("FOUND at %d\n", lsa->search.value);
            exit(lsa->search.position);
        } else {
            int32_t index = -1;
            int32_t position_relative = -1;

            if (v_pid.length == v_pid.capacity) {
                pid_t *new_base = NULL;
                size_t new_capacity = v_pid.capacity * 2;
                new_base = realloc(v_pid.base, sizeof *v_pid.base * new_capacity);
                assert(new_base);

                v_pid.base = new_base;
                v_pid.capacity = new_capacity;
            }

            v_pid.base[v_pid.length++] = pid;

            wait(&status);
            position_relative = (status / 256);

            index = position_relative 
            + (lsa->array.subcapacity * (i / lsa->array.subcapacity));

            printf("i: %d\n", i);
            printf("status: %d\n", status);
            printf("position_relative, or (status / 256): %d\n", position_relative);
            printf("index, or (status / 256) + (i / subcap): %d\n", index);
            
            printf("the \"master\" pid is %d\n\n", getpid());

            if (lsa->array.base[index] == lsa->search.key) {
                lsa->search.value = index;
                break;
            }
        }
    }

    for (i = 0; i < v_pid.length; i++) {
        printf("v_pid.base[%d]: %d\n", i, v_pid.base[i]);
    }

    {
        free(v_pid.base);
        v_pid.base = NULL;

        v_pid.capacity = 0;
        v_pid.length = 0;
    }
}

/*
void lsargs_search(lsargs_t **l) {
    lsargs_t *lsa = *(lsargs_t **)(l);

    int i = 0;

    for (i = 0; i < lsa->array.capacity; i += lsa->array.subcapacity) {
        ++lsa->search.partition;

        lsa->search.range_start = i;
        lsa->search.range_end = lsa->search.range_start + lsa->array.subcapacity;

        handler_lsearch(&lsa);

        if (lsa->search.value > -1) {
            break;
        }
    }
}
*/

void *handler_lsearch(void *arg) {
    lsargs_t *lsa = *(lsargs_t **)(arg);
    int32_t j = 0;
    int32_t position = 0;

    printf("\n");
    for (j = lsa->search.range_start; j < lsa->search.range_end; j++) {
        printf("array[%d]: %d\n", j, lsa->array.base[j]);

        if (lsa->array.base[j] == lsa->search.key) {
            lsa->search.value = j;
            lsa->search.position = position;
            break;
        }

        ++position;
    }
    printf("\n");

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
