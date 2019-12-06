/**
 *  @file       DUMBserver.c
 *  @brief      Client source file (server) for Asst3:
 *              The Decidedly Uncomplicated Message Broker
 *
 *  @author     Gemuele Aludino
 *  @date       26 Nov 2019
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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "vptr.h"
#include "user.h"
#include "network.h"

typedef struct entry entry_t;
struct entry {
    int fd;
    vptr_t *users;
};

static void *handler_connection(void *arg);
static void *handler_client(void *arg);

int usr_usage(void);

int usr_creat(vptr_t *v, int fd);
int usr_opnbx(vptr_t *v, int fd);
int usr_delbx(vptr_t *v, int fd, int index);
int usr_gdbye(vptr_t *v, int fd, int index);

int usr_opnop(vptr_t *v, int fd, int index);
int usr_putmg(vptr_t *v, int fd, int index);
int usr_nxtmg(vptr_t *v, int fd, int index);
int usr_clsbx(vptr_t *v, int fd, int index);

void temp();

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    int status = -1;
    int ssockfd = -1;

    vptr_t *users = NULL;
    entry_t entry = { -1, NULL };

    pthread_t thread_connection;
    pthread_attr_t attr_connection;

    ssocket_init(&ssockfd, AF_INET, SOCK_STREAM, 8345, 3);

    users = vptr_new(4, user_delete);

    entry.fd = ssockfd;
    entry.users = users;

    pthread_attr_init(&attr_connection);

    if ((status = pthread_create(&thread_connection, &attr_connection, handler_connection, &entry)) <
        0) {
        fprintf(stderr, "Error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&attr_connection);

    pthread_join(thread_connection, NULL);
    vptr_delete(&users);

    return EXIT_SUCCESS;
}

static void *handler_connection(void *arg) {
    struct sockaddr_in client;
    socklen_t len_client = 0;

    entry_t entry_server = { -1, NULL };
    int accept_fd = 0;

    char buffer_ipaddr[256];
    char *ip_addr = NULL;
    uint16_t portno = 0;

    int i = 0;
    int status = -1;

    struct {
        entry_t *base;
        size_t capacity;
        size_t length;
    } entry_vec = { NULL, 4, 0 };

    struct {
        pthread_t *base;
        size_t capacity;
        size_t length;
    } thread_vec = { NULL, 4, 0 };

    len_client = sizeof client;
    entry_server = *(entry_t *)(arg);

    thread_vec.base = calloc(thread_vec.capacity, sizeof *thread_vec.base);
    assert(thread_vec.base);

    entry_vec.base = calloc(entry_vec.capacity, sizeof *entry_vec.base);
    assert(entry_vec.base);

    while ((accept_fd = accept(entry_server.fd, (struct sockaddr *)(&client), &len_client))) {
        entry_t entry_client = { 0, NULL };
        pthread_attr_t attr;

        ip_addr = get_ipaddr(accept_fd, buffer_ipaddr);
        portno = get_portno(accept_fd);

        fprintf(stdout, "Connected %s via port %d\n", ip_addr, portno);

        if (i == thread_vec.capacity) {
            pthread_t *new_base = NULL;
            size_t new_capacity = thread_vec.capacity * 2;

            new_base = realloc(thread_vec.base, sizeof *new_base * new_capacity);
            assert(new_base);

            thread_vec.base = new_base;
            thread_vec.capacity = new_capacity;
        }

        if (i == entry_vec.capacity) {
            entry_t *new_base = NULL;
            size_t new_capacity = entry_vec.capacity * 2;

            new_base = realloc(entry_vec.base, sizeof *new_base * new_capacity);
            assert(new_base);

            entry_vec.base = new_base;
            entry_vec.capacity = new_capacity;
        }

        entry_client.fd = accept_fd;
        entry_client.users = entry_server.users;

        entry_vec.base[i] = entry_client;

        pthread_attr_init(&attr);

        if ((status = pthread_create(thread_vec.base + i, &attr, handler_client, entry_vec.base + i)) <
            0) {
            fprintf(stderr,
                    "Error: %s\n(unable to start client handler thread\n",
                    strerror(status));
            exit(EXIT_FAILURE);
        }

        ++i;
        thread_vec.length = i;
        entry_vec.length = i;

        pthread_attr_destroy(&attr);
    }

    if (accept_fd < 0) {
        fprintf(stderr, "Error: %s\n", strerror(accept_fd));
    }

    for (i = 0; i < thread_vec.length; i++) {
        pthread_join(thread_vec.base[i], NULL);
    }

    entry_vec.length = 0;
    entry_vec.capacity = 0;
    free(thread_vec.base);
    thread_vec.base = NULL;

    thread_vec.length = 0;
    thread_vec.capacity = 0;
    free(entry_vec.base);
    entry_vec.base = NULL;

    pthread_exit(NULL);
}

static void *handler_client(void *arg) {

    



    pthread_exit(NULL);
}
