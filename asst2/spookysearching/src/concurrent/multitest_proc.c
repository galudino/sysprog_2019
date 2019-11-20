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

void lsobject_search(lsobject_t **l) {
    lsobject_t *lso = *(lsobject_t **)(l);

    int32_t i = 0;
    int32_t j = 0;

    int32_t status = -1;
    int32_t status_found = -1;
    
    pid_t c_pid = -1;

    int32_t index = -1;
    int32_t range_start = 0;
    int32_t range_end = 0;
    int32_t partition = -1;
    int32_t position = -1;

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

    for (i = 0; i < lso->vec.capacity; i += lso->vec.subcapacity) {
        range_start = i;
        range_end = i + lso->vec.subcapacity;

        c_pid = fork();

        ++partition;

        if (c_pid == 0) {
            lso->search->value = -1;
            lso->search->range_start = range_start;
            lso->search->range_end = range_end;
            lso->search->partition = partition;
            lso->search->position = -1;             
            
            handler_lsearch(lso);

            position = lso->search->position;

            free(lso->search);
            lso->search = NULL;

            exit(position);
        } else if (c_pid > 0) {
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

            v_pid.base[v_pid.length][j++] = c_pid;
            v_pid.base[v_pid.length++][j] = partition;

            j = 0;
        } else {
            perror("fork\n");
            _exit(EXIT_FAILURE);
        }
    }

    i = 0;
    j = 0;

    while ((c_pid = waitpid(v_pid.base[++i][j], &status, 0)) > 0) {
        position = WEXITSTATUS(status);

        if (position == lso->vec.subcapacity) {
       
        } else {
            status_found = status;
            partition = v_pid.base[i][1];
            index = position + (partition * lso->vec.subcapacity);
                
            lso->search->position = position;
            lso->search->partition = partition;
            lso->search->value = index;
        }
    }

    printf("status: %d\n", status_found);
    printf("position: %d\n", lso->search->position);
    printf("index: %d\n", lso->search->value);
    printf("partition number: %d\n\n", lso->search->partition);

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
void lsobject_search(lsobject_t *l) {
    lsobject_t *lso = (lsobject_t *)(l);

    int i = 0;

    for (i = 0; i < lso->vec.capacity; i += lso->vec.subcapacity) {
        ++lso->search->partition;

        lso->search->range_start = i;
        lso->search->range_end = lso->search->range_start +
lso->vec.subcapacity;

        handler_lsearch(&lso);

        if (lso->search->value > -1) {
            break;
        }
    }
}
*/

void *handler_lsearch(void *arg) {
    lsobject_t *lso = (lsobject_t *)(arg);

    int32_t j = 0;
    int32_t position = 0;

    printf("\n");
    printf("\nBeginning search for partition %d...\n", lso->search->partition);

    for (j = lso->search->range_start; j < lso->search->range_end; j++) {
        if (lso->vec.base[j] == lso->key) {
            lso->search->value = j;
            lso->search->position = position;
            
            printf("partition %d, vec[%d]:\t%d\n", lso->search->partition, j, lso->vec.base[j]);

            printf("\n\nhandler: found %d at %d\n\n", lso->key, lso->search->value);

            break;
        }

        ++position;
    }

    lso->search->position = lso->search->value != -1 ? position : -6;

    printf("\nSearch ended for partition %d.\n", lso->search->partition);
    printf("was %s\n\n", lso->search->value != -1 ? "SUCCESSFUL" : "unsuccessful");

    return arg;
}

void *func(void *arg) {
    int *n = (int *)(arg);
    printf("did process %d\n", (*n));

    return NULL;
}

void test() {
    int i = 0;
    int status = -1;

    pid_t c_pid = 0;

    /* deploy all processes */
    while (i < TEST_NUM) {
        /* do whatever you have to do here prior to spawning procs */


        /* from here on out, each child proc will do its own work */
        c_pid = fork();

        if (c_pid == 0) {
            /* do work here, call a function, do something... */
            func(&i);

            /* exit when done */
            status = 0;
            exit(status);
        }

        ++i;
    }

    /* reap all child processes */
    while ((c_pid = waitpid(-1, &status, 0)) > 0) {
        printf("status of %d: %s (%d)\n\n", c_pid, strerror(status), status);
    }
}
