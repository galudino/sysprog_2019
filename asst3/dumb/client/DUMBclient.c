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

void *handler_inbound(void *arg);
void *handler_outbound(void *arg);

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

    bool *status_server_disconnected = NULL;

    if (argc < 3) {
        fprintf(stderr, "USAGE: %s [portnumber]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    hostname = argv[1];
    portno_str = argv[2];

    portno = atoi(portno_str);
    csockfd = csocket_open(AF_INET, SOCK_STREAM, hostname, portno);

    printf("\n\nmain menu goes here\n\n");

    pthread_attr_init(&attr_inbound);

    if ((status = pthread_create(&thread_inbound, &attr_inbound, handler_inbound, &csockfd)) < 0) {
        fprintf(stderr, "Error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    pthread_attr_init(&attr_outbound);

    if ((status = pthread_create(&thread_outbound, &attr_outbound, handler_outbound, &csockfd)) < 0) {
        fprintf(stderr, "Error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&attr_outbound);
    pthread_attr_destroy(&attr_inbound);

    pthread_join(thread_inbound, (void **)(&status_server_disconnected));

    if ((*status_server_disconnected)) {
        pthread_cancel(thread_outbound);
    } else {
        pthread_join(thread_outbound, NULL);
    }

    csocket_close(csockfd);

    return EXIT_SUCCESS;
}

void *handler_inbound(void *arg) {
    int fd = *(int *)(arg);
    
    char buffer_in[256];
    int size_read = -1;

    bool *status_server_disconnected = NULL;

    printf("started handler_inbound with fd = %d\n", fd);

    status_server_disconnected = malloc(sizeof *status_server_disconnected);
    assert(status_server_disconnected);

    (*status_server_disconnected) = false;

    while ((size_read = recv(fd, buffer_in, 256, 0)) > 0) {
        printf("server says: %s\n", buffer_in);
        bzero(buffer_in, 256);
    }

    if (size_read == 0) {
        fprintf(stdout, "Error: Server has left the network\n");
        (*status_server_disconnected) = true;
    }
    
    pthread_exit(status_server_disconnected);
}

void *handler_outbound(void *arg) {
    int fd = *(int *)(arg);

    char buffer_out[256];

    printf("started handler_outbound with fd = %d\n\n", fd);

    while (true) {
        throttle(1);
        memset(buffer_out, '\0', 256);
        strcpy(buffer_out, "==> ");
        write(STDOUT_FILENO, buffer_out, 256);
     
        memset(buffer_out, '\0', 256);
        read(STDIN_FILENO, buffer_out, 256);
        printf("You wrote: %s\n", buffer_out);

        write(fd, buffer_out, 256);
        memset(buffer_out, '\0', 256);
    }

    pthread_exit(NULL);
}
