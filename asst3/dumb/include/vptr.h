/**
 *  @file       vptr.c
 *  @brief      struct vector_ptr header file for Asst3:
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

#ifndef VPTR_H
#define VPTR_H

#include "utils.h"

#include <pthread.h>

typedef struct vector_ptr vptr_t;

vptr_t *vptr_new(size_t capacity, void (*dtor)(void *));
void vptr_delete(void *arg);

vptr_t *vptr_init(vptr_t *v, size_t capacity, void (*dtor)(void *));
vptr_t *vptr_deinit(vptr_t *v);

void vptr_pushb(vptr_t *v, void *arg);
void vptr_pushf(vptr_t *v, void *arg);
void vptr_popb(vptr_t *v);
void vptr_popf(vptr_t *v);

void vptr_insert_at(vptr_t *v, void *arg, size_t index);
void vptr_erase_at(vptr_t *v, size_t index);

void *vptr_front(vptr_t *v);
void *vptr_back(vptr_t *v);
void *vptr_at(vptr_t *v, size_t index);

bool vptr_empty(vptr_t *v);
size_t vptr_size(vptr_t *v);
size_t vptr_capacity(vptr_t *v);

int vptr_search(vptr_t *v, void *arg, int (*compare)(const void *, const void *));

int vptr_trylock(vptr_t *v);
int vptr_unlock(vptr_t *v);

void vptr_fprint(vptr_t *v, FILE *dest, void (*print)(const void *, FILE *));

#endif /* VPTR_H */