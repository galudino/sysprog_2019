/**
 *  @file       main.c
 *  @brief      Client source file
 *
 *  @author     Gemuele Aludino
 *  @date       03 Sep 2019
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>

#include "header.h"

#define ARR_SZ 256

void arrtest(void);
void structtest(void);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    arrtest();
    structtest();
    return EXIT_SUCCESS;
}

void arrtest() {
    char str[128] = "hey";
    
    /* append characters to "hey" */
    str[3] = 'y';
    str[4] = 'y';
    str[5] = '\0';
    
    /* use the rest of str to create another string */
    str[6] = 't';
    str[7] = 'e';
    str[8] = 's';
    str[9] = 't';
    str[10] = '\0';
    
    /* print both strings from the same str array */
    printf("%s %s\n", str, (str + 6));

    printf("size of str    :\t%d\n", sizeof(str));
    printf("strlen(str + 6):\t%d\n", strlen(str + 6));
    printf("strlen(str)    :\t%d\n", strlen(str));

    printf("\n");
}

void structtest() {
    point loc = { 23, 45 };
    int a = 0;
    int *b = NULL;

    printf("location: (%d, %d)\n", loc.x, loc.y);
    
    a = 2;
    b = &a;
    printf("%p\n%p\n", &a, b);
    printf("value a: %d\n", a);
    printf("deref b: %d\n", (*b));

    a = 22;
    b = &a;
    printf("%p\n%p\n", &a, b);
    printf("value a: %d\n", a);
    printf("deref b: %d\n", (*b));
}
