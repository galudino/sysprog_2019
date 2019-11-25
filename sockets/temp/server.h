/**
 *  @file       server.h
 *  @brief      Header file
 *
 *  @author     Gemuele Aludino
 *  @date       26 Nov 2018
 *  @copyright  Copyright © 2018 Gemuele Aludino. All rights reserved.
 */

#ifndef SERVER_H
#define SERVER_H

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#define ARG_COUNT_SERVER    2

#define ARGV_1_SERVER       "[portnumber]"

#define ERR_MALLOC_SSOCKINF_T "[server] Unable to malloc ssockinf."

#define ERR_INVALID_ARGS_SERVER        "[server] Invalid arguments provided."
#define ERR_FAILED_SOCKET_SERVER       "[server] Unable to create socket."
#define ERR_FAILED_BIND_SERVER         "[server] Unable to bind."
#define ERR_FAILED_SVRHNDR_SERVER      "[server] Unable to initialize server_handler thread."

#define ERR_FAILED_SIGHNDR_SERVER      "[server] Unable to initialize  signal_handler thread."

#define ERR_FAILED_CLIHNDR_SERVER      "[server] Unable to initialize client_handler thread."

#define ERR_FAILED_ACCEPT_SERVER       "[server] Unable to accept client"
#define ERR_FAILED_HANDSHAKE_SERVER    "[server] Unable to resolve client handshake."

#define QUIT_DISCONNECT "[server] Now disconnecting..."

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define ERR_MALLOC_SSOCKINF_T "[server] Unable to malloc ssockinf."

struct _server_socket_info {
    int sockfd;

    int *new_sockfd;
    uint16_t port_number;

    char *ip_address;

    struct sockaddr_in client;
    int client_addr_len;
};

typedef struct _server_socket_info ssockinf_t;

ssockinf_t *ssockinf_t_init(int sockfd);
void ssockinf_t_deinit();

void ssockinf_t_connect();

void server_connect(uint16_t port_number);

void *server_handler(void *arg);
void *client_handler(void *arg);
void *signal_handler(void *arg);

#endif /* SERVER_H */
