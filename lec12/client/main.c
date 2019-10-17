/**
 *  @file       main.c
 *  @brief      Client source file
 *
 *  @author     Gemuele Aludino
 *  @date       15 Oct 2019
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

typedef struct thing thing_t;
struct thing {
    int a;
    char b;
    int c;
    char d;
};

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
/*
        int array[16];
        int *ptr = array;
        int i = 5;

        ptr[i] = 23;
         *(ptr + i) = 256;
        *(int *)((char *)(ptr) + (sizeof *ptr * i)) = 99;

        printf("array[%d]: %d\n", i, array[i]);
*/
        thing_t t;
        /*
        t.a = 28;
        t.b = 'G';
        t.c = 29;
        t.d = 'g';
        */
        *(int *)(&t) = 28;
        *((char *)(&t) + sizeof(int)) = 'G';
        *((char *)(&t) + sizeof(int) + (sizeof(char) + 3)) = 29;
        *((char *)(&t) + sizeof(int) + (sizeof(char) + 3) + sizeof(int)) = 'g';

        printf("%d\n%c\n%d\n%c\n", t.a, t.b, t.c, t.d);
    
    /*
    int *ptr;
    const int *ptr;     // mutable ptr, immutable val
    int const *ptr;     // mutable ptr, immutable val
    int *const ptr;     // immutable ptr, mutable val
    const int *const ptr;   // immutable ptr, immutable val
    */

    {
        const int *const ptr = NULL;
    }

    return EXIT_SUCCESS;
}

