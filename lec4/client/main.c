/**
 *  @file       main.c
 *  @brief      Client source file
 *
 *  @author     Gemuele Aludino
 *  @date       10 Sep 2019
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

typedef struct node {
    int data;
    struct node *next;
} node;

void malloctest() {
    int *ptr = malloc(256 * sizeof *ptr);
    if (!ptr) {
        char str[64];
        int i = 0;
        i += sprintf(str, "malloc failure\n");
        i += sprintf(str, "exiting now...\n");
        fprintf(stderr, str);

        exit(EXIT_FAILURE);
    }
    
    /* use ptr */

    free(ptr);
    ptr = NULL;
}

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    node *head = malloc(sizeof *head);
    if (!head) {
        fprintf(stderr, "malloc failed - (struct node *)");
        exit(EXIT_FAILURE);
    }

    head->data = 28;
    head->next = malloc(sizeof *head);
    if (!head->next) {
        fprintf(stderr, "malloc failed - (struct node *)");
        exit(EXIT_FAILURE);
    }

    head->next->data = 24;
    head->next->next = NULL;

    return EXIT_SUCCESS;
}

