#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

struct vector {
    struct vector_base {
        void *start;
        void *finish;
        void *end_of_storage;
    } impl;

    size_t width;
};

typedef struct vector vector;

#define DEF_SIZE 16

vector *v_new(size_t width);
void v_delete(void *arg);
void v_pushb(vector *v, const void *valaddr);
void v_popb(vector *v);
size_t v_size(vector *v);
size_t v_capacity(vector *v);
void *v_at(vector *v, size_t n);

int main(int argc, const char *argv[]) {
    vector *v = v_new(sizeof(int));
    // begins at size 16
    
    int i = 0;
    for (i = 0; i < 100; i++) {
        v_pushb(v, &i);
    }

    for (i = 0; i < 100; i++) {
        printf("%d\n", *(int *)(v_at(v, i)));
    }

    return 0;
}

vector *v_new(size_t width) {
    vector *v = malloc(sizeof *v);
    assert(v);

    void *start = calloc(DEF_SIZE, width);
    assert(start);
    v->impl.start = start;
    v->impl.finish = v->impl.start;
    v->impl.end_of_storage = (char *)(v->impl.start) + (DEF_SIZE * width);

    v->width = width;

    return v;
}

size_t v_size(vector *v) {
    char *last = (char *)(v->impl.finish);
    char *first = (char *)(v->impl.start);

    return (last - first) / (v->width);
}

size_t v_capacity(vector *v) {
    char *last = (char *)(v->impl.end_of_storage);
    char *first = (char *)(v->impl.start);

    return (last - first) / (v->width);

}

void v_pushb(vector *v, const void *valaddr) {
    if (v->impl.finish == v->impl.end_of_storage) {
        size_t new_cap = v_capacity(v) * 2;
        void *newstart = realloc(v->impl.start, new_cap * v->width);

        v->impl.start = newstart;
        v->impl.finish = (char *)(v->impl.start) + (v_size(v) * v->width);
        v->impl.end_of_storage = (char *)(v->impl.start) + (new_cap * v->width);
    }

    // shallow copy
    memcpy(v->impl.finish, valaddr, v->width);
    v->impl.finish = (char *)(v->impl.finish) + (v->width);
}

void *v_at(vector *v, size_t n) {
    void *target = (char *)(v->impl.start) + (n * v->width);
    return target;
}
