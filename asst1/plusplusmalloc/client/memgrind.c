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

#define _POSIX_C_SOURCE 199390L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "mymalloc.h"
#include "vector.h"
#include "vector_str.h"

#define MGR__A_ITER_MAX 150

#define MGR__B_ITER_MAX 150
#define MGR__B_INTERVAL 50

#define MGR__C_ITER_MAX 50

#define MGR__D_ALLOC_MIN 1
#define MGR__D_ALLOC_MAX 64
#define MGR__D_ITER_MAX 50

#define MGR__E_MIN 29
#define MGR__E_MAX 59

#define MGR__F_MIN 11
#define MGR__F_MAX 55

#define MGR__F_RNDSTR_ARR_MIN 4
#define MGR__F_RNDSTR_ARR_MAX 8
#define MGR__F_RNDSTR_COL_MIN 4
#define MGR__F_RNDSTR_COL_MAX 8
#define MGR__F_RNDSTR_LEN_MIN 16
#define MGR__F_RNDSTR_LEN_MAX 48

#define MGR__MAX_ITER 100
#define MGR__TEST_COUNT 6

#define elapsed_time_ns(BEF, AFT)                                              \
    (((double)((pow(10.0, 9.0) * AFT.tv_sec) + (AFT.tv_nsec))) -               \
     ((double)((pow(10.0, 9.0) * BEF.tv_sec) + (BEF.tv_nsec))))

#define elapsed_time_ms(BEF, AFT)                                              \
    ((elapsed_time_ns(BEF, AFT) * pow(10.0, -6.0)))
#define elapsed_time_s(BEF, AFT) (elapsed_time_ns(BEF, AFT) * pow(10.0, -9.0))

#define convert_ns_to_s(NS) ((NS) * pow(10.0, -9.0))
#define convert_ns_to_ms(NS) ((NS) * pow(10.0, -6.0))
#define convert_ns_to_mcs(NS) ((NS) * (pow(10.0, -3.0)))

#define MCS "µs"

char *randstr(char *buffer, size_t length);

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
 *      so that you eventually malloc 50 bytes in total_ns.
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
 *      Keep track of each malloc so that all mallocs do not exceed your
 total_ns
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

/**< memgrind: testing routine */
void mgr__run_test(void (*test)(uint32_t, uint32_t, uint32_t),
                   char tch,
                   uint32_t min,
                   uint32_t max,
                   uint32_t interval,
                   FILE *dest);

/**< memgrind: tests a through f (in order) */
void mgr__simple_alloc_free(uint32_t max_iter, uint32_t alloc_sz, uint32_t dummy);
void mgr__alloc_array_interval(uint32_t max_iter, uint32_t alloc_sz, uint32_t interval);
void mgr__alloc_array_range(uint32_t max_allocs, uint32_t alloc_sz_min, uint32_t alloc_sz_max);
void mgr__char_ptr_array(uint32_t min, uint32_t max, uint32_t dummy);
void mgr__vector(uint32_t min, uint32_t max, uint32_t dummy);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status, 0 on success, else failure
 */
int main(int argc, const char *argv[]) {
    /**
     *  Results may output to a filestream of choice, if preferred.
     *  Default is stdout.
     */
    FILE *stream = stdout;

    /**
     *  Important for randomization.
     */
    srand(time(NULL));

    fprintf(stdout, "\n%s%s%s\n", KGRN_b, "mymalloc allocator stress tests", KNRM);
    fprintf(stdout,
            "Each individual test is run %lu times and wall-clock time "
            "averaged.\n\n",
            MGR__MAX_ITER);
    fprintf(stdout, "All times are expressed in microseconds (%s)\n\n", MCS);

    fprintf(stdout,
            "-------------------------------------------------------------\n");
    fprintf(stdout,
            "%s%s%s\t\t%s%s%s\t\t%s%s%s\t\t%s%s%s\n",
            KWHT_b,
            "test",
            KNRM,
            KWHT_b,
            "mean",
            KNRM,
            KWHT_b,
            "slowest",
            KNRM,
            KWHT_b,
            "total",
            KNRM);
    printf("-------------------------------------------------------------\n");

    mgr__run_test(mgr__simple_alloc_free, 'a', MGR__A_ITER_MAX, 1, 0, stream);
    mgr__run_test(mgr__alloc_array_interval, 'b', MGR__B_ITER_MAX, 1, MGR__B_INTERVAL, stream);
    mgr__run_test(mgr__alloc_array_range, 'c', MGR__C_ITER_MAX, 1, 1, stream);
    mgr__run_test(mgr__alloc_array_range, 'd', MGR__D_ITER_MAX, MGR__D_ALLOC_MIN, MGR__D_ALLOC_MAX, stream);
    mgr__run_test(mgr__char_ptr_array, 'e', MGR__E_MIN, MGR__E_MAX, 0, stream);
    mgr__run_test(mgr__vector, 'f', MGR__F_MIN, MGR__F_MAX, 0, stream);

    printf("\n");

    return EXIT_SUCCESS;
}

/**
 *  @brief function that conducts the stress test addressed by the
 *         callback function pointer test, and output
 */
void mgr__run_test(void (*test)(uint32_t, uint32_t, uint32_t),
                   char tch,
                   uint32_t min,
                   uint32_t max,
                   uint32_t interval,
                   FILE *dest) {
    struct timespec x = { 0.0, 0.0 };
    struct timespec y = { 0.0, 0.0 };

    double total_ns = 0.0;
    double elapsed_ns_avg = 0.0;
    double time_slowest = 0.0;
    double time_this = 0.0;

    uint32_t i = 0;

    total_ns = 0.0;
    for (i = 0; i < MGR__MAX_ITER; i++) {
        clock_gettime(CLOCK_REALTIME, &x);
        test(min, max, interval);
        clock_gettime(CLOCK_REALTIME, &y);

        time_this = elapsed_time_ns(x, y);
        time_slowest = time_this > time_slowest ? time_this : time_slowest;

        total_ns += time_this;
    }

    elapsed_ns_avg = total_ns / MGR__MAX_ITER;

    fprintf(dest,
            "%s%c%s\t\t%.5lf %s%s%s\t%.5lf %s%s%s\t%.5lf %s%s%s\t\t\n",
            KGRN_b,
            tch,
            KNRM,
            convert_ns_to_mcs(elapsed_ns_avg),
            KGRY,
            MCS,
            KNRM,
            convert_ns_to_mcs(time_slowest),
            KGRY,
            MCS,
            KNRM,
            convert_ns_to_mcs(total_ns),
            KGRY,
            MCS,
            KNRM);
}

void mgr__simple_alloc_free(uint32_t max_iter, uint32_t alloc_sz, uint32_t dummy) {
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

void mgr__alloc_array_interval(uint32_t max_iter, uint32_t alloc_sz, uint32_t interval) {
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

void mgr__alloc_array_range(uint32_t max_allocs, uint32_t alloc_sz_min, uint32_t alloc_sz_max) {
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
#ifndef MYMALLOC__RELEASE_MODE
                LOG(__FILE__, KMAG_b "--- max allocs met ---");
#endif
                hit_max_allocs = true;
                listlog();
            }

            offset = count.to_free - 1;

            curr = ch_ptrarr + offset;
            nonnull = (*curr) != NULL;

            if (nonnull) {
                free((*curr));
                (*curr) = NULL;

#ifndef MYMALLOC__RELEASE_MODE
                LOG(__FILE__, KYEL_b "freed");
#endif
            } else {
#ifndef MYMALLOC__RELEASE_MODE
                LOG(__FILE__, KYEL_b "skipped");
#endif
            }

            --count.to_free;
        } else {
            if (randrnge(false, true)) {
#ifndef MYMALLOC__RELEASE_MODE
                LOG(__FILE__, "will free");
#endif

                if (count.allocs > 0) {
                    offset = count.allocs - 1;

                    curr = ch_ptrarr + offset;
                    nonnull = (*curr) != NULL;

                    if (nonnull) {
                        free((*curr));
                        (*curr) = NULL;

#ifndef MYMALLOC__RELEASE_MODE
                        LOG(__FILE__, KYEL_b "freed");
#endif
                    } else {
#ifndef MYMALLOC__RELEASE_MODE
                        LOG(__FILE__, KRED_b "nothing to free");
#endif
                    }
                } else {
#ifndef MYMALLOC__RELEASE_MODE
                    LOG(__FILE__, KRED_b "nothing to free");
#endif
                }

                /*
                if (count.allocs > 0) {
                    offset = randrnge(0, count.allocs - 1);

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
                */
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

#ifndef MYMALLOC__RELEASE_MODE
                    LOG(__FILE__, "malloced");
#endif
                }
            }
        }

        ++k;
    }

    free(ch_ptrarr);
    ch_ptrarr = NULL;

#ifndef MYMALLOC__RELEASE_MODE
    printf("\ntotal iterations: %s%d%s\n\n", KYEL_b, k, KNRM);
#endif
    listlog();
}

void mgr__char_ptr_array(uint32_t min, uint32_t max, uint32_t dummy) {
    char **ch_ptrarr = NULL;
    int i = 0;

    ch_ptrarr = malloc(sizeof *ch_ptrarr * max);

    for (i = 0; i < max; i++) {
        ch_ptrarr[i] = malloc(randrnge(1, max));
    }

    for (i = 0; i < max; i++) {
        bool to_erase = randrnge(false, true);

        if (to_erase) {
            if (ch_ptrarr[i]) {
                free(ch_ptrarr[i]);
                ch_ptrarr[i] = NULL;
            }
        }
    }

#ifndef MYMALLOC__RELEASE_MODE
    listlog();
#endif

    for (i = 0; i < max; i++) {
        if (ch_ptrarr[i] == NULL) {
            int num = randrnge(min, max);
            ch_ptrarr[i] = malloc(num);
        }
    }

#ifndef MYMALLOC__RELEASE_MODE
    listlog();
#endif

    for (i = 0; i < max; i++) {
        if (ch_ptrarr[i]) {
            free(ch_ptrarr[i]);
            ch_ptrarr[i] = NULL;
        }
    }

    free(ch_ptrarr);
    ch_ptrarr = NULL;

#ifndef MYMALLOC__RELEASE_MODE
    listlog();
#endif
}

void mgr__vector(uint32_t min, uint32_t max, uint32_t dummy) {
    int i = 0;
    char buffer[MGR__F_MAX];

    vector *v = NULL;
    v = v_newr(_char_ptr_, 5);

#ifndef MYMALLOC__RELEASE_MODE
    listlog();
#endif

    for (i = 0; i < max; i++) {
        int num = randrnge(min, max);
        char *str = randstr(buffer, num);
        char *ptr = malloc(strlen(str) + 1);
        strcpy(ptr, str);

        v_pushb(v, &ptr);
    }

#ifndef MYMALLOC__RELEASE_MODE
    listlog();
#endif

    for (i = 0; i < 55; i++) {
        char **ptr = v_back(v);
        free((*ptr));
        (*ptr) = NULL;

        v_popb(v);
    }

    v_delete(&v);

#ifndef MYMALLOC__RELEASE_MODE
    listlog();
#endif
}

char *randstr(char *buffer, size_t length) {
    const char *charset = RANDSTR__CHARS;
    char *random_string = NULL;

    uint32_t key = 0;
    int n = 0;
    size_t string_length = 0;

    string_length = 26 * 2 + 10 + 7;
    random_string = buffer;

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
