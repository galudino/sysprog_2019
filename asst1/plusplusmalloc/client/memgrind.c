/**
 *  @file       memgrind.c
 *  @brief      Client source file for Asst1: ++malloc
 *
 *  @author     Gemuele Aludino
 *  @date       30 Sep 2019
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

#include <sys/time.h>

#include "mymalloc.h"
#include "rbtree.h"

void test_all(void);
void test_a(void);
void test_b(void);
void test_c(void);
void test_d(void);
void test_e(void);
void test_f(void);

#define MEMGRIND__A_ITER_MAX 150

#define MEMGRIND__B_ITER_MAX 150
#define MEMGRIND__B_INTERVAL 50

#define MEMGRIND__C_ITER_MAX 50
#define MEMGRIND__C_INTERVAL 50

#define MEMGRIND__D_ALLOC_MIN 1
#define MEMGRIND__D_ALLOC_MAX 64
#define MEMGRIND__D_INTERVAL 50

#define MEMGRIND__E_RBT_MIN  1
#define MEMGRIND__E_RBT_MAX  4

#define MEMGRIND__E_RBN_MIN  8
#define MEMGRIND__E_RBN_MAX  32


char *randstr(size_t length);

#define randrnge(min, max) \
        ((rand() % (int)(((max) + 1) - (min))) + (min))

#define RANDSTR__CHARS \
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!;"


/**
 *  memgrind unit tests
 *
 *  A:  malloc() 1 byte and immediately free it - do this 150 times

 *  B:  malloc() 1 byte, store the pointer in an array - do this 150 times
        Once you've malloc()'ed 50 byte chunks, then free() the 50 1-byte
        pointers one-by-one.

 *  C:  Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
 *      Do this until you have allocated 50 times.
 *
 *      Keep track of each operation
 *      so that you eventually malloc 50 bytes in total.
 *
 *      If you have already allocated 50 times,
 *      disregard the random and just free() on each iteration
 *
 *      Keep track of each operation
 *      so that you eventually free() all pointers.
 *      Don't allow a free() if you have no pointers to free().
 *
 *  D:  Randomly choose between a randomly-sized malloc() or free()ing a
 *      pointer -- do this many times (see below)
 *
 *      Keep track of each malloc so that all mallocs do not exceed your total
 *      memory capacity.
 *
 *      Keep track of each operation so that you eventually malloc 50 times.
 *      Keep track of each operation so that eventually free() all pointers
 *      Choose a random allocation size between 1 and 64 bytes.
 *
 *  E:  A workload of your choosing.
 *  F:  A workload of your choosing.
 *
 *      Describe both workloads in testplan.txt
 *
 *  Your memgrind.c should run all the workloads, one after another, 100 times.
 *  It should record the run time for each workload and store it.
 *
 *  When all 100 iterations of all the workloads have been run,
 *  memgrind.c should calculate the mean time for each workload to execute
 *  and output them in sequence.
 *
 *  You might find the gettimeofday(struct timeval *tv, struct timezone *tz)
 *  function in the <time.h> library useful.
 *
 *  You should run memgrind yourself and include its results in your readme.pdf.
 *  Be sure to discuss your findings, especially any interesting or unexpected
 *  results.
 */


/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status, 0 on success, else failure
 */
int main(int argc, const char *argv[]) {


    srand(time(NULL));

    test_b();


    return EXIT_SUCCESS;
}

char *randstr(size_t length) {
    const char *charset = RANDSTR__CHARS;
    char *random_string = NULL;

    uint32_t key = 0;
    int n = 0;
    size_t string_length = 0;

    string_length = 26 * 2 + 10 + 7;
    random_string = malloc(length + 1);

    if (random_string == NULL) {
        return NULL;
    }

    for (n = 0; n < length; n++) {            
        key = rand() % string_length;   
        random_string[n] = charset[key];
    }

    random_string[length] = '\0';
    return random_string;
}

void test_all(void) {

}

void test_a(void) {
    uint32_t i = 0;
    
    listlog();

    /* MEMGRIND__A_ITER_MAX == 150 */
    for (i = 0; i < MEMGRIND__A_ITER_MAX; i++) {
        char *ch = NULL;

        ch = malloc(1);

        listlog();
        
        if (ch) {
            free(ch);
            ch = NULL;
        }

        listlog();
    }
}

void test_b(void) {
    uint32_t i = 0;

    /* MEMGRIND__B_ITER_MAX == 150 */
    char *ch_ptrarr[MEMGRIND__B_ITER_MAX];
    char **sentinel = ch_ptrarr + MEMGRIND__B_INTERVAL;

    listlog();

    while (i < MEMGRIND__B_ITER_MAX) {
        char *ch = NULL;

        ch = malloc(1);

        if (ch) {
            ch_ptrarr[i++] = ch;
        }

        if ((ch_ptrarr + i) == sentinel) {
            /* MEMGRIND__B_INTERVAL == 50 */
            uint32_t j = MEMGRIND__B_INTERVAL;

            listlog();

            while (j > 0) {
                if (ch_ptrarr[i - j]) {
                    free(ch_ptrarr[i - j]);
                    ch_ptrarr[i - j] = NULL;
                }

                --j;
            }

            sentinel = (ch_ptrarr + i) + MEMGRIND__B_INTERVAL;
            listlog();
        }
    }

    listlog();
}

void test_c(void) {
    struct {
        uint16_t allocs;
        int16_t to_free;
    } count = { 0, MEMGRIND__C_ITER_MAX};

    uint16_t i = 0;
    uint16_t k = 0;

    char *ch = NULL;
    char *ch_ptrarr[MEMGRIND__C_ITER_MAX];

    bool allocated[MEMGRIND__C_ITER_MAX];

    memset(allocated, false, MEMGRIND__C_ITER_MAX);
    memset(ch_ptrarr, 0, sizeof(ch_ptrarr));

    while (count.to_free > 0) {
        char **curr = NULL;
        
        bool *alloced = NULL;
        bool nonnull = false;

        bool max_allocs_met = count.allocs == MEMGRIND__C_ITER_MAX;

        uint16_t offset = 0;

        if (max_allocs_met) {
            offset = count.to_free - 1;

            curr = ch_ptrarr + offset;
            alloced = allocated + offset;
            nonnull = (*curr);

            if ((*alloced) && nonnull) {                
                free((*curr));
                (*curr) = NULL;
                
                (*alloced) = false;
            }

            --count.to_free;
        } else {
            if (randrnge(false, true)) {
                LOG(__FILE__, "will free");
                
                if (count.allocs > 0) {
                    offset = count.allocs - 1;

                    curr = ch_ptrarr + offset;
                    alloced = allocated + offset;
                    nonnull = (*curr);

                    if ((*alloced) && nonnull) {
                        LOG(__FILE__, KYEL_b "freed");

                        free((*curr));
                        (*curr) = NULL;
                        
                        (*alloced) = false;
                    }
                }
            
            } else {
                LOG(__FILE__, "will malloc");

                ch = malloc(1);
                ch_ptrarr[count.allocs] = ch;
                allocated[count.allocs] = true;

                ++count.allocs;
            }
        }

        ++k;
    }

    printf("\ntotal iterations: %s%d%s\n\n", KYEL_b, k, KNRM);

    listlog();
}

void test_d(void) {

}

void test_e(void) {
    int i = 0;
    int j = 0;

    const int rbt_max = randrnge(MEMGRIND__E_RBT_MIN, MEMGRIND__E_RBT_MAX);
    const int rbn_max = randrnge(MEMGRIND__E_RBN_MIN, MEMGRIND__E_RBN_MAX);

    rbtree **t_arr = NULL;
    void *ptr = NULL;

    t_arr = malloc(sizeof *t_arr * rbt_max);

    listlog();

    for (i = 0; i < rbt_max; i++) {
        rbtree *t = rbtree_new();
        t_arr[i] = t ? t : NULL;
    }

    listlog();

    for (i = 0; i < rbt_max; i++) {
        for (j = 0; j < rbn_max; j++) {
            rbtree *t = t_arr[i];

            if (t) {
                rbtree_insert(t, rand());
            }
        }
    }

    for (i = 0; i < rbt_max; i++) {
        for (j = 0; j < rbn_max; j++) {
            bool to_erase = randrnge(0, 1);

            if (to_erase > 0) {
                rbtree *t = t_arr[i];
                
                if (t) {
                    rbtree_erase_min(t);
                }
            }
        }
    }

    ptr = malloc(2000);
    ptr = malloc(4097);

    listlog();
}

void test_f(void) {

}
