/**
 *  @file       vector_str.h
 *  @brief      Header file for vector ADT specialized for (char *)
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
 *  the rights to use, copy, modify, block_merge, publish, distribute, sublicense,
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
#ifndef VECTOR_STR_H
#define VECTOR_STR_H

/**
 *  @struct     vstr
 *  @brief      Represents a dynamic array ADT specialized for (char *)
 */
struct gcs__vstr {
    struct gcs__vstr_base {
        char **start;          /**< pointer to calloc'ed block of (char *) */
        char **finish;         /**< pointer to one-past the last element */
        char **end_of_storage; /**< pointer to last block of (char *) */
    } impl;
};

typedef struct gcs__vstr gcs__vstr;

#define GCS__VSTR_INITIAL_SIZE 4

/**
 *  @define GCS__VSTR_DEEP_COPY
 *  @brief  Define this directive to enable deep copies of elements into vstr
 */
#define GCS__VSTR_DEEP_COPY

/**< gcs__vstr: constructor/destructor functions */
void gcs__vstr_init(gcs__vstr *v, size_t capacity);
void gcs__vstr_deinit(gcs__vstr *v);

/**< gcs__vstr: size/capacity functions */
size_t gcs__vstr_size(gcs__vstr *v);
size_t gcs__vstr_capacity(gcs__vstr *v);
bool gcs__vstr_empty(gcs__vstr *v);

/**< gcs__vstr: element access functions */
char **gcs__vstr_at(gcs__vstr *v, size_t index);
char **gcs__vstr_front(gcs__vstr *v);
char **gcs__vstr_back(gcs__vstr *v);

/**< gcs__vstr: reallocation function */
void gcs__vstr_resize(gcs__vstr *v, size_t n);

/**< gcs__vstr: stack functions, push/pop */
void gcs__vstr_pushb(gcs__vstr *v, char **straddr);
void gcs__vstr_popb(gcs__vstr *v);

/**< gcs__vstr: clear elements */
void gcs__vstr_clear(gcs__vstr *v);

/**< gcs__vstr: print contents to stream (stdout) */
void gcs__vstr_puts(gcs__vstr *v);


#endif /* VECTOR_STR_H */