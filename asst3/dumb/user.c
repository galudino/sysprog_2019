/**
 *  @file       user.c
 *  @brief      User source file for Asst3:
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

#include "user.h"
#include "vptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

struct user {
    char *uname;
    vptr_t *msgbx;
    bool active;
};

user_t *user_new(char *uname) {
    return user_init(malloc(sizeof(user_t)), uname);
}

void user_delete(void *arg) {
    user_t **u = (user_t **)(arg);
    free(user_deinit((*u)));
    (*u) = NULL;
}

int user_compare(const void *c1, const void *c2) {
    char **key = (char **)(c1);
    user_t **val = (user_t **)(c2);

    int result = strcmp((*key), (*val)->uname);

    return result == 0 ? 0 : result;
}

void user_print(const void *arg, FILE *dest) {
    user_t **u = (user_t **)(arg);

    fprintf(dest, "\n*******************\n");

    fprintf(dest, "username: %s\nactive: %s\n", (*u)->uname, (*u)->active ? "true" : "false");
    fprintf(dest, "-------------------\n");
    
    vptr_fprint((*u)->msgbx, dest, str_print);
    fprintf(dest, "*******************\n\n");
}

user_t *user_init(user_t *u, char *uname) {
    u->uname = strdup(uname);
    u->msgbx = vptr_new(4, str_delete);
    u->active = false;

    return u;
}

user_t *user_deinit(user_t *u) {
    vptr_delete(&(u->msgbx));

    free(u->uname);
    u->uname = NULL;

    return u;
}

bool user_active(user_t *u) { return u->active; }

int user_open(user_t *u) {
    int status = 0;

    if ((status = vptr_trylock(u->msgbx)) == 0) {
        u->active = true;
    }

    return status;
}

int user_close(user_t *u) {
    int status = 0;

    if ((status = vptr_unlock(u->msgbx)) == 0) {
        u->active = false;
    }

    return status;
}

size_t user_message_count(user_t *u) { return vptr_size(u->msgbx); }

int user_message_put(user_t *u, char *message) {
    char *msg = strdup(message);
    vptr_pushb(u->msgbx, &msg);

    return 0;
}

char **user_message_peek(user_t *u) {
    char **peek = NULL;

    if (vptr_empty(u->msgbx) == false) {
        peek = vptr_front(u->msgbx);
    }

    return peek;
}

int user_message_pop(user_t *u) {
    int status = 0;

    if (vptr_empty(u->msgbx)) {
        status = -1;
    } else {
        vptr_popf(u->msgbx);
    }

    return status;
}
