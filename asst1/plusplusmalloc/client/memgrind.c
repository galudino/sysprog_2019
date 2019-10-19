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

void memgrind__simple_alloc_free(uint32_t max_iter, uint32_t alloc_sz);

void memgrind__alloc_array_interval(uint32_t max_iter, uint32_t alloc_sz, uint32_t interval);

void memgrind__alloc_random_range(uint32_t max_allocs, uint32_t alloc_sz_min, uint32_t alloc_sz_max);

void memgrind__red_black_tree(uint32_t tree_ct_min,
                              uint32_t tree_ct_max,
                              uint32_t node_ct_min,
                              uint32_t node_ct_max);
void memgrind__random_string_generator(uint32_t strarr_min,
                                         uint32_t strarr_max,
                                         uint32_t arrlen_min,
                                         uint32_t arrlen_max,
                                         uint32_t strlen_min,
                                         uint32_t strlen_max);

void test_e(void);
void test_f(void);

#define MEMGRIND__A_ITER_MAX 150

#define MEMGRIND__B_ITER_MAX 150
#define MEMGRIND__B_INTERVAL 50

#define MEMGRIND__C_ITER_MAX 50

#define MEMGRIND__D_ALLOC_MIN 1
#define MEMGRIND__D_ALLOC_MAX 64
#define MEMGRIND__D_ITER_MAX 50

#define MEMGRIND__E_RBT_MIN 2
#define MEMGRIND__E_RBT_MAX 4

#define MEMGRIND__E_RBN_MIN 8
#define MEMGRIND__E_RBN_MAX 16

#define MEMGRIND__F_RNDSTR_ARR_MIN 4
#define MEMGRIND__F_RNDSTR_ARR_MAX 8
#define MEMGRIND__F_RNDSTR_COL_MIN 4
#define MEMGRIND__F_RNDSTR_COL_MAX 8
#define MEMGRIND__F_RNDSTR_LEN_MIN 16
#define MEMGRIND__F_RNDSTR_LEN_MAX 48

#define mgr__test_a() memgrind__simple_alloc_free(MEMGRIND__A_ITER_MAX, 1)
#define mgr__test_b()                                                          \
    memgrind__alloc_array_interval(MEMGRIND__B_ITER_MAX, 1, MEMGRIND__B_INTERVAL)
#define mgr__test_c() memgrind__alloc_random_range(MEMGRIND__C_ITER_MAX, 1, 1)
#define mgr__test_d()                                                          \
    memgrind__alloc_random_range(MEMGRIND__D_ITER_MAX, MEMGRIND__D_ALLOC_MIN, MEMGRIND__D_ALLOC_MAX)

#define mgr__test_e()                                                          \
    memgrind__red_black_tree(MEMGRIND__E_RBT_MIN, MEMGRIND__E_RBT_MAX, MEMGRIND__E_RBN_MIN, MEMGRIND__E_RBN_MAX)
#define mgr__test_f()                                                          \
    memgrind__random_string_generator(MEMGRIND__F_RNDSTR_ARR_MIN,              \
                                      MEMGRIND__F_RNDSTR_ARR_MAX,              \
                                      MEMGRIND__F_RNDSTR_COL_MIN,              \
                                      MEMGRIND__F_RNDSTR_COL_MAX,              \
                                      MEMGRIND__F_RNDSTR_LEN_MIN,              \
                                      MEMGRIND__F_RNDSTR_LEN_MAX)

char *randstr(size_t length);

#define randrnge(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))

#define RANDSTR__CHARS                                                         \
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!;"

/**
 *  memgrind unit tests
 *
 *  A:  malloc() 1 byte and immediately free it - do this 150 times

 *  B:  malloc() 1 byte, store the pointer in an array - do this 150 times
        Once you've malloc()'ed 50 byte chunks, then free() the 50 1-byte
        pointers one-by-one.

 *  C:  Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
 *      Do this until you have ch_ptrarr_allocated 50 times.
 *
 *      Keep track of each operation
 *      so that you eventually malloc 50 bytes in total.
 *
 *      If you have already ch_ptrarr_allocated 50 times,
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
 *  When all 100 iterations of all the workloads have been run,Abcd10293847

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

    /*mgr__test_a();*/
    /*mgr__test_b();*/

    /*mgr__test_c();*/
    /*mgr__test_d();*/
    /*mgr__test_e();*/
    mgr__test_f();

    return EXIT_SUCCESS;
}

void memgrind__simple_alloc_free(uint32_t max_iter, uint32_t alloc_sz) {
    uint32_t i = 0;

    listlog();

    for (i = 0; i < max_iter; i++) {
        char *ch = NULL;
        ch = malloc(alloc_sz);

        if (ch) {
            free(ch);
            ch = NULL;
        }
    }

    listlog();
}

void memgrind__alloc_array_interval(uint32_t max_iter, uint32_t alloc_sz, uint32_t interval) {
    uint32_t i = 0;

    char **ch_ptrarr = NULL;
    char **sentinel = NULL;

    listlog();

    ch_ptrarr = malloc(sizeof *ch_ptrarr * max_iter);
    sentinel = ch_ptrarr + interval;

    while (i < max_iter) {
        char *ch = NULL;
        ch = malloc(alloc_sz);

        if (ch) {
            ch_ptrarr[i++] = ch;
        }

        if ((ch_ptrarr + i) == sentinel) {
            uint32_t j = interval;

            listlog();

            while (j > 0) {
                char **curr = ch_ptrarr + (i - j);

                if ((*curr)) {
                    free((*curr));
                    (*curr) = NULL;
                }

                --j;
            }

            sentinel = (ch_ptrarr + i) + interval;
            listlog();
        }
    }

    free(ch_ptrarr);
    ch_ptrarr = NULL;

    listlog();
}

void memgrind__alloc_random_range(uint32_t max_allocs, uint32_t alloc_sz_min, uint32_t alloc_sz_max) {
    struct {
        uint32_t allocs;
        int to_free;
    } count;

    uint32_t i = 0;
    uint32_t k = 0;

    char **ch_ptrarr = NULL;

    bool hit_max_allocs = false;

    count.allocs = 0;
    count.to_free = max_allocs;

    ch_ptrarr = malloc(sizeof *ch_ptrarr * max_allocs);

    memset(ch_ptrarr, 0, max_allocs);

    while (count.to_free > 0) {
        char **curr = NULL;

        bool *allocated = NULL;
        bool nonnull = false;

        bool max_allocs_met = (count.allocs == max_allocs);

        uint32_t offset = 0;

        if (max_allocs_met) {
            if (hit_max_allocs == false) {
                LOG(__FILE__, KMAG_b "--- max allocs met ---");
                hit_max_allocs = true;
                listlog();
            }

            offset = count.to_free - 1;

            curr = ch_ptrarr + offset;
            nonnull = (*curr);

            if (nonnull) {
                free((*curr));
                (*curr) = NULL;

                LOG(__FILE__, KYEL_b "freed");
            } else {
                LOG(__FILE__, KYEL_b "skipped");
            }

            --count.to_free;
        } else {
            if (randrnge(false, true)) {
                LOG(__FILE__, "will free");

                if (count.allocs > 0) {
                    offset = count.allocs - 1;

                    curr = ch_ptrarr + offset;
                    nonnull = (*curr);

                    if (nonnull) {
                        free((*curr));
                        (*curr) = NULL;

                        LOG(__FILE__, KYEL_b "freed");
                    } else {
                        LOG(__FILE__, KRED_b "nothing to free");
                    }
                } else {
                    LOG(__FILE__, KRED_b "nothing to free");
                }
            } else {
                char *ch = NULL;
                uint32_t size = 0;

                size = alloc_sz_min == alloc_sz_max ?
                           alloc_sz_min :
                           randrnge(alloc_sz_min, alloc_sz_max);

                ch = malloc(size);

                if (ch) {
                    *(ch_ptrarr + count.allocs) = ch;
                    ++count.allocs;

                    LOG(__FILE__, "malloced");
                }
            }
        }

        ++k;
    }

    free(ch_ptrarr);
    ch_ptrarr = NULL;

    printf("\ntotal iterations: %s%d%s\n\n", KYEL_b, k, KNRM);
    listlog();
}

void memgrind__red_black_tree(uint32_t tree_ct_min,
                              uint32_t tree_ct_max,
                              uint32_t node_ct_min,
                              uint32_t node_ct_max) {
    uint32_t i = 0;
    int j = 0;

    const int rbt_max = randrnge(tree_ct_min, tree_ct_max);

    rbtree **rbt_ptrarr = NULL;
    rbt_ptrarr = malloc(sizeof *rbt_ptrarr * rbt_max);

    listlog();

    memset(rbt_ptrarr, 0, sizeof *rbt_ptrarr * rbt_max);

    for (i = 0; i < rbt_max; i++) {
        rbtree *t = rbtree_new();
        rbt_ptrarr[i] = t ? t : NULL;
    }

    listlog();

    for (i = 0; i < rbt_max; i++) {
        uint32_t rbn_max = randrnge(node_ct_min, node_ct_max);
        rbtree **t = rbt_ptrarr + i;
        
        bool erase_tree = randrnge(false, true);

        if ((*t)) {
            while (j < rbn_max) {
                int num = rand();

                if (rbtree_find((*t), num) == false) {
                    rbtree_insert((*t), num);
                    ++j;
                }
            }

            while (j >= 0) {
                bool erase_node = randrnge(false, true);
                
                if (erase_node) {
                    bool erase_min = randrnge(false, true);

                    if (erase_min) {
                        rbtree_erase_min((*t));
                    } else {
                        rbtree_erase_max((*t));
                    }
                } else {
                    int num = rand();

                    if (rbtree_find((*t), num) == false) {
                        rbtree_insert((*t), num);
                    }
                }

                --j;
            }
        }

        if (erase_tree) {
            uint32_t rnd = randrnge(0, i);
            rbtree **t = rbt_ptrarr + rnd;

            if ((*t)) {
                rbtree_delete(t);
                (*t) = NULL;
                printf("erased tree %d\n", i);
            }
        }
    }

    listlog();

    for (i = 0; i < rbt_max; i++) {
        rbtree **t = rbt_ptrarr + i;

        if ((*t)) {
            rbtree_delete(t);
            (*t) = NULL;
        }
    }

    free(rbt_ptrarr);
    rbt_ptrarr = NULL;

    listlog();
}

void memgrind__random_string_generator(uint32_t strarr_min,
                                       uint32_t strarr_max,
                                       uint32_t arrlen_min,
                                       uint32_t arrlen_max,
                                       uint32_t strlen_min,
                                       uint32_t strlen_max) {
    /*
    struct counter {
        uint32_t min;
        uint32_t max;
    } strarr, arrlen, strlen;

    uint32_t i = 0;
    uint32_t j = 0;

    uint32_t arrstrarr_len = 0;
    uint32_t *arrstrarr_lengths = NULL;

    uint32_t arrstrarr_allocated[arrstrarr_len][arrlen_max];
    uint32_t **arrstrarr_allocated = NULL;

    char ***arrstrarr = NULL;
    char **strarr_curr = NULL;
    char *str_curr = NULL;

    strarr.min = strarr_min;
    strarr.max = strarr_max;
    arrlen.min = arrlen_min;
    arrlen.max = arrlen_max;
    strlen.min = strlen_min;
    strlen.max = strlen_max;

    arrstrarr_len = randrnge(strarr.min, strarr.max);
    arrstrarr_lengths = malloc(sizeof *arrstrarr_lengths * arrstrarr_len);

    arrstrarr = malloc(sizeof *arrstrarr * arrstrarr_len);
    
    arrstrarr_allocated = malloc(sizeof *arrstrarr_allocated * arrstrarr_len);
    for (i = 0; i < arrstrarr_allocated; i++) {
        arrstrarr_allocated[i] = malloc(sizeof **arrstrarr_allocated * )
    }
    
    memset(arrstrarr_allocated, false, sizeof(arrstrarr_allocated));

    
    
    for (i = 0; i < arrstrarr_len; i++) {
        uint32_t strarr_len = randrnge(arrlen.min, arrlen.max);

        strarr_curr = malloc(sizeof *strarr_curr * strarr_len);
        arrstrarr_lengths[i] = strarr_len;

        for (j = 0; j < strarr_len; j++) {
            str_curr = randstr(randrnge(strlen.min, strlen.max));

            if (str_curr) {
                strarr_curr[j] = str_curr;
                arrstrarr_allocated[i][j] = true;
            }
        }

        arrstrarr[i] = strarr_curr;
    }

    
    for (i = 0; i < arrstrarr_len; i++) {
        strarr_curr = arrstrarr[i];

        j = 0;
        for (j = 0; j < arrstrarr_lengths[i]; j++) {
            printf("string: %s\n", strarr_curr[j]);
        }

        printf("\n");
    }
    printf("---\n\n");
    

    for (i = 0; i < arrstrarr_len; i++) {
        strarr_curr = arrstrarr[i];

        j = 0;
        for (j = 0; j < arrstrarr_lengths[i]; j++) {
            bool to_free = randrnge(false, true);

            if (to_free) {
                if (strarr_curr[j]) {
                    free(strarr_curr[j]);
                    strarr_curr[j] = NULL;
                    arrstrarr_allocated[i][j] = false;
                } else {
                }
            }
        }
    }

    
    for (i = 0; i < arrstrarr_len; i++) {
        strarr_curr = arrstrarr[i];

        j = 0;
        for (j = 0; j < arrstrarr_lengths[i]; j++) {
            printf("string: %s\n", strarr_curr[j]);
        }

        printf("\n");
    }
    printf("---\n\n");
    

    for (i = 0; i < arrstrarr_len; i++) {
        strarr_curr = arrstrarr[i];

        j = 0;
        for (j = 0; j < arrstrarr_lengths[i]; j++) {
            if (arrstrarr_allocated[i][j]) {
                free(strarr_curr[j]);
                strarr_curr[j] = NULL;
                arrstrarr_allocated[i][j] = false;
            } else {
            }
        }

        free(strarr_curr);
        strarr_curr = NULL;
    }

    free(arrstrarr_lengths);
    arrstrarr_lengths = NULL;

    free(arrstrarr);
    arrstrarr = NULL;

    listlog();
    */
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

void test_e(void) {
    /*
    int i = 0;
    int j = 0;

    const int rbt_max = randrnge(MEMGRIND__E_RBT_MIN, MEMGRIND__E_RBT_MAX);

    rbtree **rbt_ptrarr = NULL;
    void *ptr = NULL;

    rbt_ptrarr = malloc(sizeof *rbt_ptrarr * rbt_max);

    listlog();

    for (i = 0; i < rbt_max; i++) {
        rbtree *t = rbtree_new();
        rbt_ptrarr[i] = t ? t : NULL;
    }

    listlog();

    for (i = 0; i < rbt_max; i++) {
        int rbn_max = randrnge(MEMGRIND__E_RBN_MIN, MEMGRIND__E_RBN_MAX);
        for (j = 0; j < rbn_max; j++) {
            rbtree *t = rbt_ptrarr[i];

            if (t) {
                rbtree_insert(t, rand());
            }
        }
    }

    for (i = 0; i < rbt_max; i++) {
        int rbn_max = randrnge(MEMGRIND__E_RBN_MIN, MEMGRIND__E_RBN_MAX);

        for (j = 0; j < rbn_max; j++) {
            bool to_erase = randrnge(0, 1);

            if (to_erase > 0) {
                rbtree *t = rbt_ptrarr[i];

                if (t) {
                    rbtree_erase_min(t);
                }
            }
        }
    }

    listlog();
    */
}

void test_f(void) {}
