/**
 *  @file       client_server.h
 *  @brief      Header file
 *
 *  @author     Gemuele Aludino
 *  @date       24 Nov 2019
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

#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#else
# define false '\0'
# define true '0'
typedef unsigned char bool;
#endif

#if WIN32 || _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

#define PORT 8345

#define KNRM "\x1B[0;0m"   /**< reset to standard color/weight */
#define KNRM_b "\x1B[0;1m" /**< standard color bold */

#define KGRY "\x1B[0;2m" /**< dark grey */

#define KITL "\x1B[0;3m" /**< italics */
#define KULN "\x1B[0;4m" /**< underline */

#define KBNK "\x1B[0;5m" /**< blink every second */
#define KHIL "\x1B[0;7m" /**< grey highlight */

#define KRED "\x1B[0;31m" /**< red */
#define KGRN "\x1B[0;32m" /**< green */
#define KYEL "\x1B[0;33m" /**< yellow */
#define KBLU "\x1B[0;34m" /**< blue */
#define KMAG "\x1B[0;35m" /**< magenta */
#define KCYN "\x1B[0;36m" /**< cyan */
#define KWHT "\x1B[0;37m" /**< white */

#define KRED_b "\x1B[1;31m" /**< red bold */
#define KGRN_b "\x1B[1;32m" /**< green bold */
#define KYEL_b "\x1B[1;33m" /**< yellow bold */
#define KBLU_b "\x1B[1;34m" /**< blue bold */
#define KMAG_b "\x1B[1;35m" /**< magenta bold */
#define KCYN_b "\x1B[1;36m" /**< cyan bold */
#define KWHT_b "\x1B[1;37m" /**< white bold */

typedef struct client_socket_info csockinfo_t;
typedef struct client_socket_input csockinput_t;
typedef struct client_socket_msg csockmsg_t;
typedef struct client_socket_handler csockhandler_t;

struct client_socket_info {
    int fd;

    struct client_socket_input {
        char *command;
        char *arg;
    } input;

    struct client_socket_message {
        char *inbound;
        char *outbound;
    } msg;

    struct client_socket_handler {
        pthread_t inbound;
        pthread_t outbound;
    } handler;
};

csockinfo_t *csockinfo_init(csockinfo_t *csock);
void *csockinfo_deinit(csockinfo_t *csock);

void *csockinfo_new(void);
void csockinfo_delete(void *arg);

int csockinfo_connect(csockinfo_t *csock, const char *hostname, int portno);
void csockinfo_connect_server(csockinfo_t *csock, const char *hostname, int portno);

void *csockinfo_handler_inbound(void *arg);
void *csockinfo_handler_outbound(void *arg);

void throttle(int seconds);

void *(*csockinfo_handler_inbound_fp)(void *);
void *(*csockinfo_handler_outbound_fp)(void *);

void (*throttle_fp)(int);

typedef struct server_socket_info ssockinfo_t;
typedef struct server_socket_fd ssockfd_t;
typedef struct server_socket_client ssockclient_t;

struct server_socket_info {
    struct server_socket_fd {
        int sock;
        int *sock_new;
    } fd;

    uint16_t portno;
    char *ipaddr;

    struct server_socket_client {
        struct sockaddr_in addr;
        int addr_len;
    } client;
};

ssockinfo_t *ssockinfo_init(ssockinfo_t *ssock, int sockfd);
void *ssockinfo_deinit(ssockinfo_t *ssock);

void *ssockinfo_new(void);
void ssockinfo_delete(void *arg);

int ssockinfo_connect(ssockinfo_t *ssock, uint16_t portno);

void *ssockinfo_handler_server(void *arg);
void *ssockinfo_handler_client(void *arg);

void *handler_signal(void *arg);

void *(*ssockinfo_handler_server_fp)(void *);
void *(*ssockinfo_handler_client_fp)(void *);

void *(*handler_signal_fp)(void *);

#endif /* CLIENT_SERVER_H */
