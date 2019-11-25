/**
 *  @file       client.h
 *  @brief      Header file
 *
 *  @author     Gemuele Aludino
 *  @date       26 Nov 2018
 *  @copyright  Copyright © 2018 Gemuele Aludino. All rights reserved.
 */

#ifndef CLIENT_H
#define CLIENT_H

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#define ARG_COUNT_CLIENT        3
#define ARGV_1_CLIENT           "[hostname]"
#define ARGV_2_CLIENT           "[portnumber]"

#define ERR_INVALID_ARGS_CLIENT    "[client] Invalid arguments provided."
#define ERR_FAILED_SOCKET_CLIENT   "[client] Unable to create socket."
#define ERR_NO_HOSTNAME_CLIENT     "[client] No such host by name"
#define ERR_ATTEMPT_CONN_CLIENT    "[client] Attempting to connect to"
#define ERR_FAILED_INBOUND_CLIENT  "[client] Unable to initialize inbound_handler thread."

#define ERR_FAILED_OUTBOUND_CLIENT "[client] Unable to initialize outbound_handler."

#define ERR_DISCONN_CLIENT         "The server has left the network. (abrupt termination)"

#define ERR_MALLOC_CSOCKINF_T "[client] Unable to malloc csockinf."

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

struct _client_socket_info {
    int sockfd;
    
    char *command;
    char *arg;
    char *inbound_message;
    char *outbound_message;

    pthread_t pt_inbound_handler;
    pthread_t pt_outbound_handler;
};

typedef struct _client_socket_info csockinf_t;

csockinf_t *csockinf_t_init();
void csockinf_t_deinit(csockinf_t **csockinf);

void csockinf_t_connect(csockinf_t **c, const char *hostname, int port_number);

void serv_connect(csockinf_t *csockinf, const char *hostname, int port_number);

void *inbound_handler(void *arg);
void *outbound_handler(void *arg);

void throttle(int sec);

#endif /* CLIENT_H */
