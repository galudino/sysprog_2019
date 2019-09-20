/**
 *  @file       main.c
 *  @brief      Client source file
 *
 *  @author     Gemuele Aludino
 *  @date       19 Sep 2019
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

#include "header.h"

int int_compare(const void *c1, const void *c2);
int str_compare(const void *c1, const void *c2);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    int *intarr = NULL;
    char **strarr = NULL;
    int i = 0;

    int (*comparator)(const void *, const void *) = NULL;

    intarr = calloc(16, sizeof *intarr);
    assert(intarr);
    
    for (i = 0; i < 16; i++) {
        intarr[i] = rand() % 50;
    }

    comparator = int_compare;
    qsort(intarr, 16, sizeof *intarr, comparator);

    for (i = 0; i < 16; i++) {
        printf("intarr[%d]: %d\n", i, intarr[i]);
    }

    free(intarr);
    intarr = NULL;
    
    puts("");

    strarr = calloc(8, sizeof *strarr);
    assert(strarr);

    strarr[0] = strcpy(malloc(strlen("delta") + 1), "delta");
    strarr[1] = strcpy(malloc(strlen("beta") + 1), "beta");
    strarr[2] = strcpy(malloc(strlen("alpha") + 1), "alpha");
    strarr[3] = strcpy(malloc(strlen("foxtrot") + 1), "foxtrot");
    strarr[4] = strcpy(malloc(strlen("charlie") + 1), "charlie");
    strarr[5] = strcpy(malloc(strlen("echo") + 1), "echo");
    strarr[6] = strcpy(malloc(strlen("icecream") + 1), "icecream");
    strarr[7] = strcpy(malloc(strlen("kilo") + 1), "kilo");
    strarr[8] = strcpy(malloc(strlen("gilgamesh") + 1), "gilgamesh");
    
    comparator = str_compare;
    qsort(strarr, 9, sizeof *strarr, comparator);

    for (i = 0; i < 9; i++) {
        printf("strarr[%d]: %s\n", i, strarr[i]);
    }

    for (i = 0; i < 9; i++) {
        free(strarr[i]);
        strarr[i] = NULL;
    }

    free(strarr);
    strarr = NULL;
        
    return EXIT_SUCCESS;
}

int int_compare(const void *c1, const void *c2) {
    int *first = (int *)(c1);
    int *second = (int *)(c2);

    return (*first) - (*second);
}

int str_compare(const void *c1, const void *c2) {
    char **first = (char **)(c1);
    char **second = (char **)(c2);

    return strcmp((*first), (*second));
}

