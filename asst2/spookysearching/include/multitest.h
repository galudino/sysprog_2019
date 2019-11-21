/**
 *  @file       multitest.h
 *  @brief      Header file for Asst2: Spooky Search
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

#ifndef MULTITEST_H
#define MULTITEST_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "header.h"

#define TEST_NUM    10

#define lsearch(lsobject_addr)    lsobject_search(lsobject_addr)

#define ARR_SEARCH_VALUE    99
#define ARR_RANGE_START     (256)
#define ARR_RANGE_END       ((65536) + (1))
#define ARR_RANGE_START_SUB (16)
#define ARR_RANGE_END_SUB   ((250) + (1))

typedef struct linear_search_object lsobject_t;
typedef struct linear_search_vector lsvector_t;
typedef struct linear_search_params lsparams_t;

struct linear_search_object {
    struct linear_search_vector {
        int32_t *base;
        int32_t capacity;
        int32_t subcapacity;
    } *vec;

    struct linear_search_params {
        int32_t value;
        int32_t range_start;
        int32_t range_end;
        int32_t partition;
        int32_t position;
    } search; 

    int32_t key;
};

void lsobject_search(lsobject_t **l);

void test();

#endif /* MULTITEST_H */
