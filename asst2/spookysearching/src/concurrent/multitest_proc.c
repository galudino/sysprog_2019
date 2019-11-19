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

void lsargs_search(lsargs_t *l) {
    lsargs_t *lsa = (lsargs_t *)(l);

    int32_t i = 0;
    int32_t j = 0;
    int32_t status = -1;
    int32_t status_found = -1;
    
    pid_t pid = -1;

    int32_t index = -1;
    int32_t position = -1;
    int32_t partition = -1;

    struct {
        pid_t (*base)[2];
        size_t capacity;
        size_t length;
    } v_pid = { NULL, 16, 0 };

    {
        v_pid.base = calloc(16, sizeof *v_pid.base);
        assert(v_pid.base);
        v_pid.capacity = 16;
        v_pid.length = 0;
    }

    v_pid.base[v_pid.length][j++] = getpid();
    v_pid.base[v_pid.length++][j] = partition;

    j = 0;

    for (i = 0; i < lsa->array.capacity; i += lsa->array.subcapacity) {
        lsa->search.range_start = i;
        lsa->search.range_end = i + lsa->array.subcapacity;

        pid = fork();

        ++partition;

        if (pid == 0) {
            lsa->search.partition = partition;
            handler_lsearch(lsa);
            exit(lsa->search.position);
        } else if (pid > 0) {
            {
                if (v_pid.length == v_pid.capacity) {
                    pid_t (*new_base)[2] = NULL;
                    size_t new_capacity = v_pid.capacity * 2;

                    new_base = realloc(v_pid.base, sizeof *v_pid.base * new_capacity);

                    assert(new_base);

                    v_pid.base = new_base;
                    v_pid.capacity = new_capacity;
                }
            }

            v_pid.base[v_pid.length][j++] = pid;
            v_pid.base[v_pid.length++][j] = partition;

            j = 0;
        } else {
            perror("fork\n");
            _exit(EXIT_FAILURE);
        }
    }

    i = 0;
    j = 0;

    while ((pid = waitpid(v_pid.base[++i][j], &status, 0)) > 0) {
        position = WEXITSTATUS(status);

        if (position == lsa->array.subcapacity) {
       
        } else {
            status_found = status;
            partition = v_pid.base[i][1];
            index = position + (partition * lsa->array.subcapacity);

            lsa->search.position = position;
            lsa->search.partition = partition;
            lsa->search.value = index;
        }
    }

    printf("status: %d\n", status_found);
    printf("position: %d\n", lsa->search.position);
    printf("index: %d\n", lsa->search.value);
    printf("partition number: %d\n\n", lsa->search.partition);

    j = 0;

    for (i = 0; i < v_pid.length; i++) {
        for (j = 0; j < 2; j++) {
            printf("v_pid.base[%d][%d]: %d\n", i, j, v_pid.base[i][j]);
        }

        printf("\n");
    }

    {
        free(v_pid.base);
        v_pid.base = NULL;

        v_pid.capacity = 0;
        v_pid.length = 0;
    }
}

/*
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
*/

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

    lsa->search.position = lsa->search.value != -1 ? position : -6;

    printf("\nSearch ended for partition %d.\n", lsa->search.partition);
    printf("was %s\n\n", lsa->search.value != -1 ? "SUCCESSFUL" : "unsuccessful");

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
