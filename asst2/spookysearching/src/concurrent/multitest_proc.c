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

static void *handler_lsearch(void *arg);

int __linear_search_int32__(int32_t *base, size_t capacity, int32_t subcapacity, int32_t key) {
    lsobject_t *lso = NULL;

    int32_t i = 0;
    int32_t j = 0;

    int32_t status = -1;

    pid_t c_pid = -1;

    int32_t index = -1;
    int32_t range_start = 0;
    int32_t range_end = 0;
    int32_t partition = -1;
    int32_t position = -1;

    struct {
        pid_t (*base)[2];
        size_t length;
    } v_pid = { NULL, 0 };

    size_t process_count = 0;

    printf("Multiprocess linear search (-proc)\n");

    {
        lso = malloc(sizeof *lso);
        assert(lso);

        lso->vec.base = base;
        lso->vec.capacity = capacity;
        lso->vec.subcapacity = subcapacity;

        lso->key = key;

        lso->search.partition = partition;
        lso->search.position = -1;
        lso->search.range_start = range_start;
        lso->search.range_end = range_end;
        lso->search.value = index;
    }

    process_count = lso->vec.capacity % lso->vec.subcapacity == 0 ?
                        (lso->vec.capacity / lso->vec.subcapacity) + 1 :
                        (lso->vec.capacity / lso->vec.subcapacity) + 2;

    {
        pid_t(*base)[2] = NULL;

        base = calloc(process_count, sizeof *base);
        assert(base);

        v_pid.base = base;
    }

    v_pid.base[v_pid.length][j++] = getpid();
    v_pid.base[v_pid.length++][j] = partition;

    j = 0;

    for (i = 0; i < lso->vec.capacity; i += lso->vec.subcapacity) {
        range_start = i;
        range_end = i + lso->vec.subcapacity;

        ++partition;

        c_pid = fork();

        if (c_pid == 0) {
            lsobject_t *lso_process = NULL;

            {
                lso_process = malloc(sizeof *lso_process);
                assert(lso_process);

                lso_process->vec.base = lso->vec.base;
                lso_process->vec.capacity = lso->vec.capacity;
                lso_process->vec.subcapacity = lso->vec.subcapacity;

                lso_process->search.value = -1;
                lso_process->search.range_start = range_start;
                lso_process->search.range_end = range_end;
                lso_process->search.partition = partition;
                lso_process->search.position = -1;

                lso_process->key =  lso->key;
            }

            handler_lsearch(&lso_process);

            position = lso_process->search.position;

            {
                v_pid.length = 0;

                free(v_pid.base);
                v_pid.base = NULL;
            }

            {
                {
                    lso_process->search.partition = 0;
                    lso_process->search.position = -1;
                    lso_process->search.range_end = 0;
                    lso_process->search.range_start = 0;
                    lso_process->search.value = -1;
                }

                lso_process->key = 0;

                free(lso_process);
                lso_process = NULL;
            }

            {
                {
                    lso->search.partition = 0;
                    lso->search.position = -1;
                    lso->search.range_end = 0;
                    lso->search.range_start = 0;
                    lso->search.value = -1;
                }

                lso->key = 0;

                {
                    lso->vec.subcapacity = 0;
                    lso->vec.capacity = 0;
                }

                free(lso);
                lso = NULL;
            }

            exit(position);
        } else if (c_pid > 0) {
            v_pid.base[v_pid.length][j++] = c_pid;
            v_pid.base[v_pid.length++][j] = partition;

            j = 0;

            if (lso->search.value > -1) {
                break;
            }
        } else {
            perror("fork\n");
            _exit(EXIT_FAILURE);
        }
    }

    i = 0;

    for (i = 1; i < v_pid.length; i++) {
        waitpid(v_pid.base[i][0], &status, 0);

        position = WEXITSTATUS(status);

        if (position == lso->vec.subcapacity) {

        } else {
            int32_t offset = -1;

            partition = v_pid.base[i][1];
            offset = partition * lso->vec.subcapacity;
            index = offset + position;

            lso->search.position = position;
            lso->search.partition = partition;
            lso->search.value = index;
        }
    }

    /*
    printf("position: %d\nindex: %d\npartition number: %d\n\n",
           lso->search.position,
           lso->search.value,
           lso->search.partition);
    */

    /*
    {
        for (i = 0; i < v_pid.length; i++) {
            for (j = 0; j < 2; j++) {
                printf("v_pid.base[%d][%d]: %d\n", i, j, v_pid.base[i][j]);
            }

            printf("\n");
        }
    }
    */

    {
        free(v_pid.base);
        v_pid.base = NULL;

        v_pid.length = 0;
    }

    free(lso);
    lso = NULL;

    return index;
}

/* vanilla lsobject_search, for one process only */
/*
void lsobject_search(lsobject_t **l) {
    lsobject_t *lso = *(lsobject_t **)(l);

    int32_t i = 0;

    int32_t range_start = 0;
    int32_t range_end = 0;
    int32_t partition = -1;

    signal(SIGUSR1, handler_time);

    for (i = 0; i < lso->vec.capacity; i += lso->vec.subcapacity) {
        range_start = i;
        range_end = i + lso->vec.subcapacity;

        ++partition;

        lso->search.value = -1;
        lso->search.range_start = range_start;
        lso->search.range_end = range_end;
        lso->search.partition = partition;
        lso->search.position = -1;

        handler_lsearch(l);

        if (lso->search.value > -1) {
            break;
        }
    }

    printf("position: %d\nindex: %d\npartition number: %d\n\n",
lso->search.position, lso->search.value, lso->search.partition);
}
*/

int __linear_search_int32__CONTROL__(int32_t *base, size_t capacity, int32_t subcapacity, int32_t key) {
    lsobject_t *lso = NULL;

    int32_t index = -1;
    int32_t range_start = 0;
    int32_t range_end = capacity;
    int32_t partition = subcapacity;

    {
        lso = malloc(sizeof *lso);
        assert(lso);

        lso->vec.base = base;
        lso->vec.capacity = capacity;
        lso->vec.subcapacity = subcapacity;

        lso->key = key;

        lso->search.partition = partition;
        lso->search.position = -1;
        lso->search.range_start = range_start;
        lso->search.range_end = range_end;
        lso->search.value = index;
    }

    printf("Single process linear search\n");

    handler_lsearch(&lso);
    index = lso->search.value;

    /*
    printf("position: %d\nindex: %d\npartition number: %d\n\n",
           lso->search.position,
           lso->search.value,
           lso->search.partition);
    */

    free(lso);
    lso = NULL;
 
    return index;
}

static void *handler_lsearch(void *arg) {
    lsobject_t *lso = *(lsobject_t **)(arg);

    int32_t j = 0;
    int32_t position = 0;

    /*
    printf("\n\nBeginning search for partition %d...\n",
    lso->search.partition);
    */

    for (j = lso->search.range_start; j < lso->search.range_end; j++) {
        /*
        printf("vec[%d]: %d\n", j, lso->vec.base[j]);
        */

        if (lso->vec.base[j] == lso->key) {
            lso->search.value = j;

            /*
            printf("handler: found key %d at index "
                   "%d, partition %d\n\n",
                   lso->key,
                   lso->search.value,
                   lso->search.partition);
            */

            /*
            raise(SIGUSR1);
            */

            break;
        }

        ++position;
    }

    lso->search.position = lso->search.value != -1 ? position : lso->vec.subcapacity;

    /*
    printf("\nSearch ended for partition %d.\nwas %s\n\n",
    lso->search.partition, lso->search.value != -1 ? "SUCCESSFUL" :
    "unsuccessful");
    */

    return arg;
}
