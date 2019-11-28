/**
 *  @file       network.h
 *  @brief      Network header file for Asst3:
 *              The Decidedly Uncomplicated Message Broker
 *
 *  @author     Gemuele Aludino
 *  @date       27 Nov 2019
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

#ifndef NETWORK_H
#define NETWORK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

#include "utils.h"

#define PORT 8345

typedef struct client_socket csocket_t;
typedef struct client_socket_input csocket_input_t;
typedef struct client_socket_message csocket_message_t;
typedef struct client_socket_thread csocket_thread_t;
typedef struct client_socket_handler csocket_handler_t;

/**
 *  @struct client_socket
 *  @brief TODO
 */
struct client_socket {
    int fd;

    struct client_socket_input {
        char *command;
        char *arg;
    } input;

    struct client_socket_message {
        char *inbound;
        char *outbound;
    } message;

    struct client_socket_thread {
        pthread_t inbound;
        pthread_t outbound;
    } thread;

    struct client_socket_handler {
        void *(*inbound)(void *);
        void *(*outbound)(void *);
    } handler;
};

/**< csocket_t: construct */
csocket_t *csocket_init(csocket_t *cs, 
                        void *(*inbound_handler)(void *), 
                        void *(*outbound_handler)(void *));

/**< csocket_t: destruct */
csocket_t *csocket_deinit(csocket_t *cs);

/**< csocket_t: allocate and construct */
void *csocket_new(void *(*inbound_handler)(void *), 
                  void *(*outbound_handler)(void *));

/**< csocket_t: destruct and deallocate */
void csocket_delete(void *arg);

/**< csocket_t: connect to a host */
int csocket_connect(csocket_t *cs, 
                    const char *hostname, 
                    int portno);

/**< csocket_t: deploy threads to start client */
int csocket_start(csocket_t *cs, void *(*function)(void *));

typedef struct server_socket ssocket_t;
typedef struct server_socket_fd ssocket_fd_t;
typedef struct server_socket_client_info ssocket_client_info_t;
typedef struct server_socket_thread ssocket_thread_t;
typedef struct server_socket_handler ssocket_handler_t;

/**
 *  @struct server_socket
 *  @brief TODO
 */
struct server_socket {
    struct server_socket_fd {
        int sock;
        int *sock_new;
    } fd;

    uint16_t portno;
    char *ipaddr;

    struct server_socket_client_info {
        struct sockaddr_in addr;
        int addr_len;
    } client_info;

    struct server_socket_thread {
        pthread_t server;
        pthread_t signal;
    } thread;

    struct server_socket_handler {
        void *(*server)(void *);
        void *(*signal)(void *);
    } handler;
};

/**< ssocket_t: construct */
ssocket_t *ssocket_init(ssocket_t *ss, 
                        void *(*server_handler)(void *), 
                        void *(*signal_handler)(void *));

/**< ssocket_t: destruct */
ssocket_t *ssocket_deinit(ssocket_t *ss);

/**< ssocket_t: allocate and construct */
void *ssocket_new(void *(*server_handler)(void *), 
                  void *(*signal_handler)(void *));

/**< ssocket_t: destruct and deallocate */
void ssocket_delete(void *arg);

/**< ssocket_t: connect a client */
int ssocket_connect(ssocket_t *ss, uint16_t portno);

#endif /* NETWORK_H */
