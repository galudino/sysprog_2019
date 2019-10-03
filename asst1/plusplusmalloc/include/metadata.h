#ifndef METADATA_H
#define METADATA_H

#include "utils.h"

struct metadata {
    uint32_t block_size; /* 4 */
    bool is_free;       /* 1 */
};

typedef struct metadata metadata;

#endif /* METADATA_H */