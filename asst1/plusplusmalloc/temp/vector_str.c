/**
 *  @file       vector_str.c
 *  @brief      Source file for vector ADT specialized for (char *)
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

#include "mymalloc.h"
#include "vector_str.h"

/**
 *  @brief  "Constructor" function, initializes vstr
 *
 *  @param[in]  v           pointer to vstr
 *  @param[in]  capacity    capacity desired for vstr
 */
void gcs__vstr_init(gcs__vstr *v, size_t capacity) {
    char **start = NULL;

    if (capacity <= 0) {
        WARNING(__FILE__,
                "Provided input capacity was less than or equal to "
                "0. Will default to capacity of 1.");
        capacity = 1;
    }

    start = calloc(capacity, sizeof *v->impl.start);
    massert_calloc(start);

    v->impl.start = start;
    v->impl.finish = v->impl.start;
    v->impl.end_of_storage = v->impl.start + capacity;
}

/**
 *  @brief  "Destructor" function, deinitializes vector
 *
 *  @param[in]  v   pointer to vstr
 *
 *  If deep copies are enabled, any memory allocated by vstr
 *  will be released -- otherwise, the client will be responsible
 *  for any memory to be freed per element.
 */
void gcs__vstr_deinit(gcs__vstr *v) {
    gcs__vstr_clear(v);

    free(v->impl.start);
    v->impl.start = NULL;
    v->impl.finish = NULL;
    v->impl.end_of_storage = NULL;
}

/**
 *  @brief  Returns the logical length of vstr
 *
 *  @param[in]  v   pointer to vstr
 *
 *  @return     logical length of vstr
 */
size_t gcs__vstr_size(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.finish - v->impl.start;
}

/**
 *  @brief  Returns the amount of blocks allocated for vstr's buffer
 *
 *  @param[in]  v   pointer to vstr
 *
 *  @return     capacity of vstr
 */
size_t gcs__vstr_capacity(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.end_of_storage - v->impl.start;
}

/**
 *  @brief  Determines if vstr is empty, or not
 *
 *  @param[in]  v   pointer to vstr
 *
 *  @return     true if v->impl.start == v->impl.finish, false otherwise
 */
bool gcs__vstr_empty(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start == v->impl.finish;
}

/**
 *  @brief  Retrieves the address of an element from vstr at index
 *
 *  @param[in]  v       pointer to vstr
 *  @param[in]  index   index of desired element
 *
 *  @return     address of element at index (dereference for front element)
 */
char **gcs__vstr_at(gcs__vstr *v, size_t index) {
    massert_ptr(v);
    return index < gcs__vstr_size(v) ? (v->impl.start + index) : (NULL);
}

/**
 *  @brief  Retrieves the address of vstr's front element
 *
 *  @param[in]  v       pointer to vstr
 *
 *  @return     v->impl.start (dereference for front element)
 */
char **gcs__vstr_front(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start != v->impl.finish ? (v->impl.start) : NULL;
}

/**
 *  @brief  Retrieves the address of vstr's back element
 *
 *  @param[in]  v       pointer to vstr
 *
 *  @return     v->impl.start (dereference for back element)
 */
char **gcs__vstr_back(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start != v->impl.finish ? (v->impl.finish - 1) : NULL;
}

/**
 *  @brief  Resizes vstr to size n
 *
 *  @param[in]  v   pointer to vstr
 *  @param[in]  n   desired size for vstr
 *
 *  If n is less than the current logical length (vstr_size(v)),
 *  v's buffer will be truncated (excess elements are destroyed),
 *  otherwise, v's capacity will be extended.
 */
void gcs__vstr_resize(gcs__vstr *v, size_t n) {
    char **new_start = NULL;
    size_t size = gcs__vstr_size(v);
    size_t old_capacity = gcs__vstr_capacity(v);

#ifdef GCS__VSTR_DEEP_COPY

    if (n < size) {
        char **sentinel = v->impl.start + n;
        while (--v->impl.finish != sentinel) {
            free(*(v->impl.finish));
            *(v->impl.finish) = NULL;
        }
    }

#endif

    new_start = realloc(v->impl.start, sizeof *new_start * n);
    massert_realloc(new_start);

    size = n < old_capacity ? n : size;

    v->impl.start = new_start;
    v->impl.finish = v->impl.start + size;
    v->impl.end_of_storage = v->impl.start + n;
}

/**
 *  @brief  Appends an element to the rear of vstr
 *
 *  @param[in]  v       pointer to vstr
 *  @param[in]  val     element to be copied
 *
 *  If deep copies are enabled using the directive GCS__VSTR_DEEP_COPY,
 *  (*straddr) will be duplicated into vstr's buffer and will be
 *  freed during vstr_popb/vstr_clear/vstr_deinit --
 *  otherwise, a shallow copy by assignment is made.
 */
void gcs__vstr_pushb(gcs__vstr *v, char **straddr) {
    massert_ptr(v);

    if (v->impl.finish == v->impl.end_of_storage) {
        size_t capacity = gcs__vstr_capacity(v);
        gcs__vstr_resize(v, capacity * 2);
    }

#ifdef GCS__VSTR_DEEP_COPY

    *(v->impl.finish) = malloc(strlen((*straddr)) + 1);
    massert_malloc(*(v->impl.finish));
    strcpy(*(v->impl.finish++), (*straddr));

#else

    *(v->impl.finish++) = (*straddr);

#endif
}

/**
 *  @brief  Removes element at the rear of the vector
 *
 *  @param[in]  v   pointer to vstr
 *
 *  If deep copies are enabled using the directive GCS__VSTR_DEEP_COPY,
 *  any memory at the rear of vstr's buffer will be freed --
 *  otherwise, v->impl.finish is decremented and will be overwritten
 *  when a new address takes its place.
 */
void gcs__vstr_popb(gcs__vstr *v) {
#ifdef GCS__VSTR_DEEP_COPY

    if (gcs__vstr_empty(v) == false) {
        free(*(v->impl.finish));
        *(v->impl.finish) = NULL;

        --v->impl.finish;
    }

#else

    v->impl.finish -= gcs__vstr_empty(v) == false ? 1 : 0;

#endif
}

/**
 *  @brief  Destroys elements from within v, but vstr's buffer will persist.
 *
 *  @param[in]  v   pointer to vstr
 */
void gcs__vstr_clear(gcs__vstr *v) {
    massert_ptr(v);

#ifdef GCS__VSTR_DEEP_COPY

    while (--v->impl.finish != v->impl.start) {
        free(*(v->impl.finish));
        *(v->impl.finish) = NULL;
    }

    free(*(v->impl.start));
    *(v->impl.start) = NULL;

#else

    v->impl.finish = v->impl.start;

#endif
}

/**
 *  @brief  Print the contents of vstr to stdout
 */
void gcs__vstr_puts(gcs__vstr *v) {
    char **start = v->impl.start;
    char **pos = start;

    LOG(__FILE__, "***** gcs__vstr begin *****");

    while ((pos = (start++)) != v->impl.finish) {
        LOG(__FILE__, *(pos));
    }

    LOG(__FILE__, "***** gcs__vstr end   *****\n");
}
