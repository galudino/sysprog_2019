#ifndef GCS_RBTREE_H
#define GCS_RBTREE_H

#include "utils.h"
#include "iterator.h"

#include "metadata.h"

#define BYTE_ALIGNMENT   16
typedef char alignment[BYTE_ALIGNMENT];




extern struct iterator_table *_rbtree_iterator_;

#endif /* GCS_RBTREE_H */