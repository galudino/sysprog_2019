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
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

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

statcode_t usr_creat(vptr_t *v, char *arg, int fd);
statcode_t usr_opnbx(vptr_t *v, user_t **user, char *arg, int fd, bool *box_open);
statcode_t usr_delbx(vptr_t *v, char *arg, int fd);
statcode_t usr_gdbye(vptr_t *v, user_t **user, int fd);
statcode_t usr_putmg(user_t *user, char *arg, int arglen, int fd, bool *box_open);
statcode_t usr_nxtmg(user_t *user, int fd, bool *box_open);
statcode_t usr_clsbx(user_t **user, int fd, bool *box_open, char *cmdarg);
statcode_t usr_usage(int fd);
statcode_t usr_error(int fd);

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

    const char *portno_str = NULL;
    uint16_t portno = 0;

    vptr_t *users = NULL;
    entry_t entry = { -1, NULL };

    pthread_t thread_connection;
    pthread_attr_t attr_connection;

    if (argc < 2) {
        fprintf(stderr, "USAGE: %s [port number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    portno_str = argv[1];

    portno = atoi(portno_str);
    ssockfd = ssocket_open(AF_INET, SOCK_STREAM, portno, 1024);

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

    ssocket_close(ssockfd);

    return EXIT_SUCCESS;
}

static void *handler_connection(void *arg) {
    struct sockaddr_in client;
    socklen_t len_client = 0;

    /*
    struct hostent *hp = NULL;
    char *haddrp = NULL;
    uint16_t portno = 0;
    */

    entry_t entry_server = { -1, NULL };
    int accept_fd = 0;

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

    entry_server = *(entry_t *)(arg);

    thread_vec.base = calloc(thread_vec.capacity, sizeof *thread_vec.base);
    assert(thread_vec.base);

    entry_vec.base = calloc(entry_vec.capacity, sizeof *entry_vec.base);
    assert(entry_vec.base);

    while (true) {
        pthread_attr_t attr;

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

        len_client = sizeof client;
        bzero((char *)(&client), len_client);

        accept_fd = accept(entry_server.fd, (struct sockaddr *)(&client), &len_client);

        /*
        hp = gethostbyaddr((const char *)(&client.sin_addr.s_addr),
        sizeof(client.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(client.sin_addr);
        portno = get_portno(accept_fd);
        */

        /*
        fprintf(stdout, "CONNECTED %s (%s) via port %d\n\n", hp->h_name, haddrp,
        portno);
        */

        entry_vec.base[i].fd = accept_fd;
        entry_vec.base[i].users = entry_server.users;

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
    entry_t *entry = (entry_t *)(arg);
    int fd = entry->fd;

    char buffer_in[256];
    char buffer_out[256];

    char datetime[256];
    char ipaddr[256];
    const char *description = NULL;

    int size_read = -1;

    char *cmdarg = NULL;
    ssize_t arglen = 0;

    dumbcmd_t cmddumb = ERROR_CODENO;
    statcode_t stat = _WHAT_STATNO;

    bool exit_graceful = false;
    bool goodbye = false;
    bool box_open = false;

    user_t *user_current = NULL;

    bzero(buffer_in, 256);
    bzero(buffer_out, 256);

    description = "connected";
    printf("%s %s %s\n", datetime_format(datetime), get_ipaddr(fd, ipaddr), description);

    while ((size_read = recv(fd, buffer_in, 256, 0)) > 0) {
        /* read incoming client message here */
        /*
        printf("client says: %s\n", buffer_in);
        */

        cmdarg = buffer_in;
        cmddumb = cmdarg_interpret(buffer_in, &cmdarg, &arglen);

        description = cmd_dumb[cmddumb];

        switch (cmddumb) {
        case HELLO_CODENO:
            write(fd, strcpy(buffer_out, statcode[_OK_STATNO]), 256);
            stat = _OK_STATNO;
            break;

        case GDBYE_CODENO:
            stat = _OK_STATNO;
            goodbye = true;
            break;

        case CREAT_CODENO:
            stat = usr_creat(entry->users, cmdarg, fd);
            break;

        case OPNBX_CODENO:
            stat = usr_opnbx(entry->users, &user_current, cmdarg, fd, &box_open);
            break;

        case NXTMG_CODENO:
            stat = usr_nxtmg(user_current, fd, &box_open);
            break;

        case PUTMG_CODENO:
            stat = usr_putmg(user_current, cmdarg, arglen, fd, &box_open);
            break;

        case DELBX_CODENO:
            stat = usr_delbx(entry->users, cmdarg, fd);
            break;

        case CLSBX_CODENO:
            stat = usr_clsbx(&user_current, fd, &box_open, cmdarg);
            break;

        case USAGE_CODENO:
            stat = usr_usage(fd);
            break;

        case ERROR_CODENO:
            stat = usr_error(fd);
            break;

        default:
            stat = usr_error(fd);
            break;
        }

        if (stat == _OK_STATNO) {
            fprintf(stdout, "%s %s %s\n", datetime_format(datetime), ipaddr, description);
        } else {
            description = statcode[stat];

            fprintf(stderr, "%s %s ER:%s\n", datetime_format(datetime), ipaddr, description);
        }

        if (goodbye) {
            stat = usr_gdbye(entry->users, &user_current, fd);
            break;
        }

        /*
        sprintf(buffer_out, "I got your message, it said: %s", buffer_in);
        write(fd, buffer_out, 256);
        */

        bzero(buffer_in, 256);
        bzero(buffer_out, 256);

        vptr_fprint(entry->users, stdout, user_print);
    }

    if (exit_graceful == false) {
        description = "disconnected";
        printf("%s %s %s\n", datetime_format(datetime), ipaddr, description);
    }

    close(fd);

    pthread_exit(NULL);
}

/**
 *  @brief  TODO
 *
 *  @param[in]  v
 *  @param[in]  arg
 *  @param[in]  fd
 *
 *  @return     statcode values:
 *              _OK_STATNO      user was successfully created and added to v
 *              EXIST_STATNO    box with name arg already exists in v
 *              _WHAT_STATNO    malformed message
 */
statcode_t usr_creat(vptr_t *v, char *arg, int fd) {
    statcode_t stat = _OK_STATNO;
    char buffer[256];

    const size_t arglen = strlen(arg);
    bool in_range = arglen >= 5 && arglen <= 25;

    bzero(buffer, 256);

    if (vptr_trylock(v) == 0) {
        if (in_range && isalpha(arg[0]) != 0) {
            user_t *user = NULL;

            user = user_new(arg);
            vptr_pushb(v, &user);

            stat = _OK_STATNO;
        } else {
            stat = _WHAT_STATNO;
        }

        vptr_unlock(v);
    } else {
        stat = _WHAT_STATNO;
    }

    strcpy(buffer, statcode[stat]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief      TODO
 *
 *  @param[in]   v
 *  @param[out]  user
 *  @param[in]   arg
 *  @param[in]   fd
 *  @param[out]  box_open
 *
 *  @return     statcode values:
 *              _OK_STATNO          box requested successfully opened
 *              NEXST_STATNO        box requested nonexistent
 *              OPEND_STATNO        box requested already open by another client
 *              _WHAT_STATNO        malformed message
 */
statcode_t usr_opnbx(vptr_t *v, user_t **user, char *arg, int fd, bool *box_open) {
    statcode_t stat = _WHAT_STATNO;
    char buffer[256];

    int found = -1;

    bzero(buffer, 256);

    /* if (*user) is non-null, there is an open box for this thread */
    if ((*user)) {
        stat = OPEND_STATNO;
    } else {
        /* attempt to find the user, based on arg */
        found = vptr_search(v, &arg, user_compare);

        /* if the user was found... */
        if (found >= 0) {
            user_t *u = NULL;

            /* ...retrieve the user */
            u = *(user_t **)(vptr_at(v, found));

            /* if the user is in use by another thread */
            if (user_active(u)) {
                stat = OPEND_STATNO;
            /* if the user is not in use by another thread */
            } else {
                /* if the user was successfully opened */
                if (user_open(u) == 0) {
                    (*user) = u;
                    (*box_open) = true;
                    stat = _OK_STATNO;
                }

                /* if the user could not be opened, due to an error */
            }
        /* if the user was not found */
        } else {
            stat = NEXST_STATNO;
        }
    }

    strcpy(buffer, statcode[stat]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  v
 *  @param[in]  arg
 *  @param[in]  fd
 *
 *  @return     statcode values:
 *              _OK_STATNO       box requested successfully deleted
 *              OPEND_STATNO     box requested is currently open
 *              NOTMT_STATNO     box requested is not empty
 *              _WHAT_STATNO     malformed message
 */
statcode_t usr_delbx(vptr_t *v, char *arg, int fd) {
    statcode_t stat = _WHAT_STATNO;
    char buffer[256];

    int found = -1;

    user_t *u = NULL;

    bzero(buffer, 256);

    found = vptr_search(v, &arg, user_compare);

    /* if the user was found... */
    if (found >= 0) {
        u = *(user_t **)(vptr_at(v, found));

        /* if the user is in use by any thread */
        if (user_active(u)) {
            stat = OPEND_STATNO;
        /* if the user is inactive */
        } else {
            /* if the user vector was successfully locked */
            if (vptr_trylock(v) == 0) {
                vptr_erase_at(v, found);
                vptr_unlock(v);

                stat = _OK_STATNO;
            /* if the user vector was not successfully locked */
            } else {
                stat = _WHAT_STATNO;
            }
        }
    } else {
        stat = NEXST_STATNO;
    }

    strcpy(buffer, statcode[stat]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  v
 *  @param[in]  user
 *  @param[in]  fd
 *
 *  @return     statcode values:
 *              _OK_STATNO      (no reply will actually be sent)
 */
statcode_t usr_gdbye(vptr_t *v, user_t **user, int fd) {
    statcode_t stat = _OK_STATNO;
    char buffer[256];

    bzero(buffer, 256);

    strcpy(buffer, statcode[_OK_STATNO]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  user
 *  @param[in]  arg
 *  @param[in]  arglen
 *  @param[in]  fd
 *  @param[out] box_open
 *
 *  @return     statcode values:
 *              _OK_STATNO      message successfully added to box
 *              NOOPN_STATNO    no box currently open clientside
 *              _WHAT_STATNO    malformed message
 */
statcode_t usr_putmg(user_t *user, char *arg, int arglen, int fd, bool *box_open) {
    statcode_t stat = _WHAT_STATNO;
    char buffer[256];

    bzero(buffer, 256);

    strcpy(buffer, statcode[_OK_STATNO]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  user
 *  @param[in]  fd
 *  @param[out] box_open
 *
 *  @return     statcode values:
 *              _OK_STATNO    next message successfully retrieved
 *              EMPTY_STATNO  currently open message box is empty
 *              NOOPN_STATNO  no box currently open clientside
 *              _WHAT_STATNO  malformed message
 */
statcode_t usr_nxtmg(user_t *user, int fd, bool *box_open) {
    statcode_t stat = _WHAT_STATNO;
    char buffer[256];

    bzero(buffer, 256);

    strcpy(buffer, statcode[_OK_STATNO]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  user
 *  @param[in]  fd
 *  @param[out] box_open
 *  @param[in]  cmdarg
 *
 *  @return     statcode values:
 *              _OK_STATNO    open message box was successfuly closed
 *              NOOPN_STATNO  no box currently open clientside
 *              _WHAT_STATNO  malformed message
 */
statcode_t usr_clsbx(user_t **user, int fd, bool *box_open, char *cmdarg) {
    statcode_t stat = _WHAT_STATNO;
    char buffer[256];

    bzero(buffer, 256);

    if (user_active((*user))) {
        if (user_close((*user)) == 0) {
            (*user) = NULL;
            (*box_open) = false;

            stat = _OK_STATNO;
        } else {
            printf("here\n");
            stat = _WHAT_STATNO;
        }
    } else {
        stat = _WHAT_STATNO;
    }

    strcpy(buffer, statcode[stat]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  fd
 *
 *  @return     statcode values:
 *              _OK_STATNO      permission granted to print usage menu
 *              _WHAT_STATNO    malformed message
 */
statcode_t usr_usage(int fd) {
    statcode_t stat = _WHAT_STATNO;
    char buffer[256];

    bzero(buffer, 256);

    strcpy(buffer, statcode[_OK_STATNO]);
    write(fd, buffer, 256);

    return stat;
}

/**
 *  @brief  TODO
 *
 *  @param[in]  fd
 *
 *  @return     statcode values:
 *              _OK_STATNO    request received is confirmed to be malformed
 */
statcode_t usr_error(int fd) {
    statcode_t code = _WHAT_STATNO;
    char buffer[256];

    bzero(buffer, 256);

    strcpy(buffer, statcode[_OK_STATNO]);
    write(fd, buffer, 256);

    return code;
}
