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

enum BOOL {
    NO, YES
};

typedef enum BOOL BOOL;

int pointers(int argc, const char *argv[]);
int typedef_func(int argc, const char *argv[]);
int using_const(int argc, const char *argv[]);
int morestuff(int argc, const char *argv[]);

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
    pointers(argc, argv);
    typedef_func(argc, argv);
    typedef_func(argc, argv);
    using_const(argc, argv);
    */

    morestuff(argc, argv);

    return EXIT_SUCCESS;
}

int pointers(int argc, const char *argv[]) {
    int a = 23;
    int *ptr = &a;
    int deref = (*ptr);

    int **pptr = &ptr; /* pointer to (pointer to (int)) */

    for (a = 0; a < argc; a++) {
        printf("argv[%d]: %s\n", a, argv[a]);
    }

    return EXIT_SUCCESS;
}

int typedef_func(int argc, const char *argv[]) {
    typedef int counter;
    typedef char string[256];

    counter iterate = 0;
    string str = "hey";

    while (iterate < argc) {
        printf("%s\n", argv[iterate++]);
    }

    printf("str: %s\n", str);

    return EXIT_SUCCESS;
}

int using_const(int argc, const char *argv[]) {
    const int immutable_val_0 = 23;
    int const immutable_val_1 = 23;

    const int *immutable_val_mutable_addr_0 = NULL;
    int const *immutable_val_mutable_addr_1 = NULL;

    const int *const immutable_val_immutable_addr_0 = NULL;
    int const *const immutable_val_immtable_addr_1 = NULL;
    const int const *immutable_val_immutable_addr_2 = NULL;

    const int a = 2;
    int *hackit = &a;

    *hackit = (*hackit) + 1;

    printf("a         = %d\n", a);
    printf("(*hackit) = %d\n", (*hackit));

    return EXIT_SUCCESS;
}

int morestuff(int argc, const char *argv[]) {
    size_t cap = 256;
    size_t len = 0;
    int i = 0;
    char *mybuffer = malloc(cap);
    assert(mybuffer);

    free(mybuffer);
    mybuffer = NULL;

    return EXIT_SUCCESS;
}
