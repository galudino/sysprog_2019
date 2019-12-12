/**
 *  @file       DUMBclient.c
 *  @brief      Client source file (client) for Asst3:
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>

#include "network.h"

#define ENABLE_RECONNECT true

void test();

void *handler_inbound(void *arg);
void *handler_outbound(void *arg);

dumbcmd_t last_cmd = ERROR_CODENO;

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
    int csockfd = -1;

    const char *hostname = NULL;
    const char *portno_str = NULL;
    uint16_t portno = 0;

    pthread_t thread_inbound;
    pthread_t thread_outbound;

    pthread_attr_t attr_inbound;
    pthread_attr_t attr_outbound;

    bool *server_disconnected = NULL;

    if (argc < 3) {
        fprintf(stderr, "USAGE: %s [portnumber]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    hostname = argv[1];
    portno_str = argv[2];

    portno = atoi(portno_str);

#if ENABLE_RECONNECT
connect:
#endif

    csockfd = csocket_open(AF_INET, SOCK_STREAM, hostname, portno);

    pthread_attr_init(&attr_inbound);

    if ((status = pthread_create(&thread_inbound, &attr_inbound, handler_inbound, &csockfd)) < 0) {
        fprintf(stderr, "error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    pthread_attr_init(&attr_outbound);

    if ((status = pthread_create(&thread_outbound, &attr_outbound, handler_outbound, &csockfd)) < 0) {
        fprintf(stderr, "error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&attr_outbound);
    pthread_attr_destroy(&attr_inbound);

    pthread_join(thread_inbound, (void *)(&server_disconnected));

    if ((*server_disconnected)) {
        pthread_cancel(thread_outbound);

#if ENABLE_RECONNECT
        goto connect;
#endif

    } else {
        pthread_join(thread_outbound, NULL);
    }

    /* allocated in handler_inbound */
    free(server_disconnected);
    server_disconnected = NULL;

    csocket_close(csockfd);
    printf("\n");

    printf("[client exited]\n\n");

    return EXIT_SUCCESS;
}

void *handler_inbound(void *arg) {
    int fd = *(int *)(arg);

    char buffer_in[256];
    int size_read = -1;

    bool *server_disconnected = NULL;

    bzero(buffer_in, 256);

    server_disconnected = malloc(sizeof *server_disconnected);
    assert(server_disconnected);

    (*server_disconnected) = false;

    while ((size_read = recv(fd, buffer_in, 256, 0)) > 0) {
        printf("server says: %s\n", buffer_in);
        bzero(buffer_in, 256);
        throttle(1);
        printf("almost ready...\n\n");
    }

    if (size_read == 0) {
        if (last_cmd != GDBYE_CODENO) {
            (*server_disconnected) = true;
            fprintf(stdout, "[connection severed - server has left the network]\n");
        } else {
            fprintf(stdout, "[connection severed at user's request]\n");
        }
    }

    pthread_exit(server_disconnected);
}

void *handler_outbound(void *arg) {
    int fd = *(int *)(arg);

    char buffer_out[256];
    bool started = false;

    while (last_cmd != GDBYE_CODENO) {
        bzero(buffer_out, 256);

        if (started) {
            strcpy(buffer_out, "[ready]\n==> ");
        } else {
            strcpy(buffer_out, "[type 'start' and hit RETURN to initialize the client.]\n==> ");
        }
        
        write(STDOUT_FILENO, buffer_out, 256);
        bzero(buffer_out, 256);

        if (started) {
            last_cmd = cmdarg_capture(buffer_out);
        } else {
            read(STDIN_FILENO, buffer_out, 256);
            str_trim(buffer_out, "\n");

            if (strcmp(buffer_out, cmd_engl[0]) == 0) {
                bzero(buffer_out, 256);

                strcpy(buffer_out, cmd_dumb[0]);
                started = true;
                last_cmd = HELLO_CODENO;
            } else {
                printf("\nYou must type 'start' and hit RETURN in order to proceed.\nPlease try again.\n");
            }
        }

        printf("\nWill send to server: %s\n", buffer_out);

        if (last_cmd == GDBYE_CODENO) {
            printf("\n[PLEASE WAIT]\n");
        } else {
            printf("\n[PLEASE WAIT]\nserver is busy...\nwait for [ready] ==> indicator.\n\n");
        }

        throttle(1);

        write(fd, buffer_out, 256);
        bzero(buffer_out, 256);

        throttle(2);

        if (last_cmd == HELLO_CODENO) {
            printf("[client started]\n");
        }
    }

    pthread_exit(NULL);
}
