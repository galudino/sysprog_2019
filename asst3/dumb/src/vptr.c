/**
 *  @file       vptr.c
 *  @brief      struct vector_ptr source file for Asst3:
 *              The Decidedly Uncomplicated Message Broker
 *
 *  @author     Gemuele Aludino
 *  @date       02 Dec 2019
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

#include "vptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

struct vector_ptr {
    void **base;

    size_t capacity;
    size_t length;

    pthread_mutex_t lock;

    void (*dtor)(void *);
};

vptr_t *vptr_new(size_t capacity, void (*dtor)(void *)) {
    return vptr_init(malloc(sizeof(vptr_t)), capacity, dtor);
}

void vptr_delete(void *arg) {
    vptr_t **v = (vptr_t **)(arg);
    free(vptr_deinit((*v)));
    (*v) = NULL;
}

vptr_t *vptr_init(vptr_t *v, size_t capacity, void (*dtor)(void *)) {    
    pthread_mutex_init(&v->lock, NULL);

    v->dtor = dtor;
    v->length = 0;
    v->capacity = capacity;

    v->base = calloc(v->capacity, sizeof *v->base);
    assert(v->base);

    return v;
}

vptr_t *vptr_deinit(vptr_t *v) {
    size_t i = 0;

    for (i = 0; i < v->length; i++) {
        v->dtor(v->base + i);
    }

    free(v->base);
    v->base = NULL;

    v->capacity = 0;
    v->length = 0;
    v->dtor = NULL;

    pthread_mutex_destroy(&v->lock);

    return v;
}

void vptr_pushb(vptr_t *v, void *arg) {
    if (v->length == v->capacity) {
        size_t new_capacity = 0;
        void **new_base = NULL;

        new_capacity = v->capacity * 2;
        new_base = realloc(v->base, sizeof *v->base * new_capacity);
        assert(new_base);

        v->base = new_base;
        v->capacity = new_capacity;
    }

    memcpy(v->base + v->length, arg, sizeof *v->base);
    ++v->length;
}

void vptr_pushf(vptr_t *v, void *arg) {
    if (v->length == v->capacity) {
        size_t new_capacity = 0;
        void **new_base = NULL;

        new_capacity = v->capacity * 2;
        new_base = realloc(v->base, sizeof *v->base * new_capacity);
        assert(new_base);

        v->base = new_base;
        v->capacity = new_capacity;
    }

    memmove(v->base + 1, v->base, sizeof *v->base * v->length);

    memcpy(v->base, arg, sizeof *v->base);
    ++v->length;
}

void vptr_popb(vptr_t *v) {
    v->dtor(v->base + (v->length - 1));
    v->base[v->length - 1] = NULL;

    --v->length;
}

void vptr_popf(vptr_t *v) {
    v->dtor(v->base);
    v->base[0] = NULL;

    --v->length;
    
    memmove(v->base, v->base + 1, sizeof *v->base * v->length);
}

void vptr_insert_at(vptr_t *v, void *arg, size_t index) {
    if (index >= v->length) {
        return;
    }

    if (v->length == v->capacity) {
        size_t new_capacity = 0;
        void **new_base = NULL;

        new_capacity = v->capacity * 2;
        new_base = realloc(v->base, sizeof *v->base * new_capacity);
        assert(new_base);

        v->base = new_base;
        v->capacity = new_capacity;
    }

    memmove(v->base + (index + 1), v->base + (index), sizeof *v->base * (v->length - index));

    memcpy(v->base + index, arg, sizeof *v->base);
    ++v->length;
}

void vptr_erase_at(vptr_t *v, size_t index) {
    if (v->length == 0 || index >= v->length) {
        return;
    }

    v->dtor(v->base + index);
    v->base[index] = NULL;

    --v->length;
    memmove(v->base + index, v->base + (index + 1), sizeof *v->base * (v->length - index));
}

void *vptr_front(vptr_t *v) {
    return v->base;
}

void *vptr_back(vptr_t *v) {
    return v->base + (v->length - 1);
}

void *vptr_at(vptr_t *v, size_t index) {
    if (index >= v->length) {
        return NULL;
    }

    return v->base + index;
}

bool vptr_empty(vptr_t *v) {
    return v->length == 0;
}

size_t vptr_size(vptr_t *v) {
    return v->length;
}

size_t vptr_capacity(vptr_t *v) {
    return v->capacity;
}

int vptr_search(vptr_t *v, void *arg, int (*compare)(const void *, const void *)) {
    int i = 0;
    int size = (int)(v->length);
    bool found = false;

    for (i = 0; i < size; i++) {
        if (compare(arg, v->base + i) == 0) {
            found = true;
            break;
        }
    }

    return found ? i : -1;
}

int vptr_trylock(vptr_t *v) {
    return pthread_mutex_trylock(&v->lock);
}

int vptr_unlock(vptr_t *v) {
    return pthread_mutex_unlock(&v->lock);
}

void vptr_fprint(vptr_t *v, FILE *dest, void (*print)(const void *, FILE *)) {
    size_t i = 0;

    if (vptr_empty(v)) {
        fprintf(dest, "\n-- empty --\n");
    }

    for (i = 0; i < v->length; i++) {
        print(v->base + i, dest);
        fprintf(dest, "\n");
    }

    fprintf(dest, "\n");
}
