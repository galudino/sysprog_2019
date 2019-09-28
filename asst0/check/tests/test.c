/**
 *  @file       test.c
 *  @brief      Test source file for Asst0
 *
 *  @author     Gemuele Aludino
 *  @date       19 Sep 2019
 *  @copyright  Copyright © 2019 Gemuele Aludino
 */
/**
 *  Copyright © 20xx Gemuele Aludino
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

/*
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#else
# define false '\0'
# define true '0'
typedef unsigned char bool;
#endif

#if WIN32 || _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>
*/

#include "header.h"

struct gcs__typetable {
    size_t width;
    
    void *(*copy)(void *);
    void (*dtor)(void *);
    void (*swap)(void *, void *);

    int (*compare)(const void *, const void *);
    void (*print)(const void *, FILE *dest);
};

typedef char *str;

void *gcs__str_copy(void *arg);
void gcs__str_dtor(void *arg);
void gcs__str_swap(void *c1, void *c2);

int gcs__str_compare(const void *c1, const void *c2);
void gcs__str_print(const void *arg, FILE *dest);

struct gcs__typetable ttbl_str;
struct gcs__typetable *_str_;

struct gcs__vstr {
    struct gcs__vstr_base {
        char **start;
        char **finish;
        char **end_of_storage;
    } impl;

    struct gcs__typetable *ttbl;
};

typedef struct gcs__vstr gcs__vstr;



gcs__vstr *gcs__vstr_allocate(void);
void gcs__vstr_init(vstr *v, size_t capacity);
void gcs__vstr_deinit(vstr *v);

gcs__vstr *gcs__vstr_new(void);
gcs__vstr *gcs__vstr_newr(size_t capacity);
void gcs__vstr_delete(vstr **v);

char **gcs__vstr_at(vstr *v, size_t n);
char **gcs__vstr_front(vstr *v);
char **gcs__vstr_back(vstr *v);
char ***gcs__vstr_data(vstr *v);

size_t gcs__vstr_size(vstr *v);
size_t gcs__vstr_capacity(vstr *v);
bool gcs__vstr_empty(vstr *v);

void gcs__vstr_resize(vstr *v, size_t n);
void gcs__vstr_shrinktofit(vstr *v);

void gcs__vstr_pushb(vstr *v, const char **straddr);
void gcs__vstr_popb(vstr *v);

void gcs__vstr_clear(vstr *v);

void gcs__vstr_puts(vstr *v);
void gcs__vstr_putsf(vstr *v, const char *before, const char *after, const char *postelem, const char *empty, size_t breaklim);
void gcs__vstr_fputs(vstr *v, FILE *dest);
void gcs__vstr_fputsf(vstr *v, FILE *dest, const char *before, const char *after, const char *postelem, const char *empty, size_t breaklim);


/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    /* Enter source code here... */



    return EXIT_SUCCESS;
}

gcs__vstr *gcs__vstr_allocate(void) {
    vstr *v = NULL;
    v = malloc(sizeof *v);
    return v;
}