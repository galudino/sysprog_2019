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

void intro(void);
void pointers(void);
void structsunions(void);
void arrays(void);
void pointersandarrays(void);

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
    intro();
    pointers();
    structsunions();
    arrays();
    */

    pointersandarrays();
    return EXIT_SUCCESS;
}

void intro() {
    int c = 2;

    int *ptr = &c, z;

    printf("%d\n", z);

    c = 2;
    printf("%d\n", *(&c));

}

void pointers() {
    int c = 2;
    char a = 'a';

    int *cptr = &c;
    char *aptr = &a;

    /* sizeof(void *) is 8 bytes (64 bits) */
    printf("%d %d\n", sizeof(cptr), sizeof(aptr));
}

void structsunions() {
    struct thingy {
        char a;
        int b;
    };

    union stuff {
        char a;
        int b;
    };
    
    /* unions have memory for all fields */
    struct thingy test0 = { 'a', 1984 };
    
    /* no good */
    /* union stuff test1 = { 'a', 1984 }; */
    
    /* unions have memory for the largest field */
    union stuff test1 = { 'a' };

    struct thingy *tp0 = &test0;
    struct thingy *tp1 = (struct thingy *)(&test0.a);
    
    /* printf("%c\n%c\n", (*tp0).a, (*tp1).a); */
    printf("%c\n%c\n", tp0->a, tp1->a);
}

void arrays() {
    int iarray[] = { 11, 22, 33, 44 };
    int i = 2;
    double darray[] = { 11.0, 22.0, 33.0, 44.0 };

    /* the addr of an array is the same addr as that of
     * the array's first element */
    printf("iarray[0] = %d\n", *(iarray));
    printf("iarray[i] = %d\n", *(iarray + i));
    /* iarray is the base address of the array. */

    printf("darray[0] = %f\n", *(darray));
    printf("darray[i] = %f\n", *(darray + i));
    /* darray is the base address of the array. */

    /* just do this instead if you want the values: */
    printf("iarray[0] = %d\n", iarray[0]);
    printf("iarray[i] = %d\n", iarray[i]);
    /* iarray is the base address of the array. */

    printf("darray[0] = %f\n", darray[0]);
    printf("darray[i] = %f\n", darray[i]);
    /* darray is the base address of the array. */
}

void pointersandarrays() {
    int a = 3;
    int *ptr = &a;

    printf("%d\n", ptr[0]);
}
