/**
 *  @file       source.c
 *  @brief      Source file
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

#include "header.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

/**< client directives */
#define ARG_COUNT_CLIENT 3
#define ARGV_1_CLIENT "[hostname]"
#define ARGV_2_CLIENT "[portnumber]"

#define ERR_INVALID_ARGS_CLIENT "[client] Invalid arguments provided."
#define ERR_FAILED_SOCKET_CLIENT "[client] Unable to create socket."
#define ERR_NO_HOSTNAME_CLIENT "[client] No such host by name"
#define ERR_ATTEMPT_CONN_CLIENT "[client] Attempting to connect to"
#define ERR_FAILED_INBOUND_CLIENT                                              \
    "[client] Unable to initialize inbound_handler thread."

#define ERR_FAILED_OUTBOUND_CLIENT                                             \
    "[client] Unable to initialize outbound_handler."

#define ERR_DISCONN_CLIENT                                                     \
    "The server has left the network. (abrupt termination)"

#define ERR_MALLOC_csockinfo_T "[client] Unable to malloc csockinfo_t."

/**< server directives */
#define ARG_COUNT_SERVER 2
#define ARGV_1_SERVER "[portnumber]"

#define ERR_INVALID_ARGS_SERVER "[server] Invalid arguments provided."
#define ERR_FAILED_SOCKET_SERVER "[server] Unable to create socket."
#define ERR_FAILED_BIND_SERVER "[server] Unable to bind."
#define ERR_FAILED_SVRHNDR_SERVER                                              \
    "[server] Unable to initialize server_handler thread."

#define ERR_FAILED_SIGHNDR_SERVER                                              \
    "[server] Unable to initialize  signal_handler thread."

#define ERR_FAILED_CLIHNDR_SERVER                                              \
    "[server] Unable to initialize client_handler thread."

#define ERR_FAILED_ACCEPT_SERVER "[server] Unable to accept client"
#define ERR_FAILED_HANDSHAKE_SERVER                                            \
    "[server] Unable to resolve client handshake."

#define QUIT_DISCONNECT "[server] Now disconnecting..."

#define ERR_MALLOC_ssockinfo_T "[server] Unable to malloc ssockinfo_t."

csockinfo_t *csockinfo_init(csockinfo_t *csock) {
    /* TODO */

    return csock;
}

void *csockinfo_deinit(csockinfo_t *csock) {
    /* TODO */

    return csock;
}

void *csockinfo_new(void) {
    /* TODO */

    return NULL;
}

void csockinfo_delete(void *arg) {
    /* TODO */


}

int csockinfo_connect(csockinfo_t *csock,
                     const char *hostname,
                     int portno) {
    /* TODO */

    return 0;
}

void csockinfo_connect_server(csockinfo_t *csock,
                             const char *hostname,
                             int portno) {
    /* TODO */
}

void *csockinfo_handler_inbound(void *arg) {
    /* TODO */

    return NULL;
}

void *csockinfo_handler_outbound(void *arg) {
    /* TODO */

    return NULL;
}

void throttle(int seconds) {
    /* TODO */


}

ssockinfo_t *ssockinfo_init(ssockinfo_t *ssock,
                         int sockfd) {
    /* TODO */

    return ssock;
}
void *ssockinfo_deinit(ssockinfo_t *ssock) {
    /* TODO */

    return ssock;
}

void *ssockinfo_new(void) {
    /* TODO */

    return NULL;
}

void ssockinfo_delete(void *arg) {
    /* TODO */
}

int ssockinfo_connect(ssockinfo_t *ssock,
                     uint16_t portno) {
    /* TODO */

    return 0;
}

void *ssockinfo_handler_server(void *arg) {
    /* TODO */

    return NULL;
}

void *ssockinfo_handler_client(void *arg) {
    /* TODO */

    return NULL;
}

void *handler_signal(void *arg) {
    /* TODO */

    return NULL;
}
