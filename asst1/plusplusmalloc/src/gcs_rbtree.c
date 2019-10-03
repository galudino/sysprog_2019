#include "gcs_rbtree.h"

static iterator rbti_begin(void *arg);
static iterator rbti_end(void *arg);

static iterator rbti_next(iterator it);
static iterator rbti_next_n(iterator it, int n);

static iterator rbti_prev(iterator it);
static iterator rbti_prev_n(iterator it, int n);

int rbti_distance(iterator *first, iterator *last);

iterator *rbti_advance(iterator *it, int n);
iterator *rbti_incr(iterator *it);
iterator *rbti_decr(iterator *it);

void *rbti_curr(iterator it);
void *rbti_start(iterator it);
void *rbti_finish(iterator it);

bool rbti_has_next(iterator it);
bool rbti_has_prev(iterator it);

struct iterator_table itbl_rbtree = {
    rbti_begin,
    rbti_end,
    rbti_next,
    rbti_next_n,
    rbti_prev,
    rbti_prev_n,
    rbti_advance,
    rbti_incr,
    rbti_decr,
    rbti_curr,
    rbti_start,
    rbti_finish,
    rbti_distance,
    rbti_has_next,
    rbti_has_prev,
    NULL
};

struct iterator_table *_rbtree_iterator_ = &itbl_rbtree;

static iterator rbti_begin(void *arg) {
    iterator it;


    return it;
}

static iterator rbti_end(void *arg) {
    iterator it;
    return it;
}

static iterator rbti_next(iterator it) {
    return it;
}

static iterator rbti_next_n(iterator it, int n) {
    return it;
}

static iterator rbti_prev(iterator it) {
    return it;
}

static iterator rbti_prev_n(iterator it, int n) {
    return it;
}

int rbti_distance(iterator *first, iterator *last) {
    return 0;
}

iterator *rbti_advance(iterator *it, int n) {
    return NULL;
}

iterator *rbti_incr(iterator *it) {
    return NULL;
}

iterator *rbti_decr(iterator *it) {
    return NULL;
}

void *rbti_curr(iterator it) {
    return NULL;
}

void *rbti_start(iterator it) {
    return NULL;
}

void *rbti_finish(iterator it) {
    return NULL;
}

bool rbti_has_next(iterator it) {
    return false;
}

bool rbti_has_prev(iterator it) {
    return false;
}