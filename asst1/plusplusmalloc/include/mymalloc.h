/**
 *  @file       mymalloc.h
 *  @brief      Header file for Asst1: ++malloc
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

#ifndef MY_MALLOC_H
#define MY_MALLOC_H

/* #define MYMALLOC__RELEASE_MODE */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#else
# define false  0
# define true   1
typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
#endif
*/

#if WIN32 || _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>

#include "utils.h" 

#define MYMALLOC__BLOCK_SIZE    4096
extern char myblock[MYMALLOC__BLOCK_SIZE];

#ifndef _STDLIB_H
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1
#define size_t          unsigned long int
#define malloc(size)    mymalloc(size, __FILE__, __LINE__)
#define free(ptr)       myfree(ptr, __FILE__, __LINE__)
#endif

/**< mymalloc: memory allocator functions, allocate and free */
void *mymalloc(size_t size, const char *filename, size_t lineno);
void myfree(void *ptr, const char *filename, size_t lineno);







#endif /* MY_MALLOC_H */
