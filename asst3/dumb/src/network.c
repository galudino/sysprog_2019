/**
 *  @file       network.c
 *  @brief      Network source file for Asst3:
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

#include "network.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int ssocket_init(int *ssockfd, int domain, int type, uint16_t portno, int backlog) {
    struct sockaddr_in server;
    int status_bind = -1;

    memset(&server, '\0', sizeof server);

    (*ssockfd) = socket(domain, type, 0);

    if ((*ssockfd) == -1) {
        fprintf(stderr, "Unable to create server socket\n");
        exit(EXIT_FAILURE);
    }

    server.sin_family = domain;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(portno);

    status_bind = bind((*ssockfd), (struct sockaddr *)(&server), sizeof server);

    if (status_bind != 0) {
        fprintf(stderr, "Unable to bind server socket\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Now listening on port %d\n", portno);
    fflush(stdout);

    return listen((*ssockfd), backlog);
}

int csocket_init(int *csockfd, int domain, int type) {
    (*csockfd) = socket(domain, type, 0);
    return (*csockfd);
}

void csocket_connect(int *csockfd, int domain, const char *hostname, uint16_t portno) {
    struct sockaddr_in addr_server;
    struct hostent *server = NULL;

    int status = -1;
    int count_period = 3;

    memset(&addr_server, '\0', sizeof addr_server);

    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr, "Error: No hostname by identifier %s\n", hostname);
        exit(EXIT_FAILURE);
    }


    addr_server.sin_family = domain;
    addr_server.sin_port = htons(portno);

    fprintf(stdout, "Attempting to connect to %s via port %d\n", hostname, portno);

    while (true) {
        status = connect((*csockfd), (struct sockaddr *)(&addr_server), sizeof addr_server);

        if (status != -1) {
            fprintf(stdout, "Connected (%s via port %d)\n", hostname, portno);
            break;
        } else {
            if (count_period == 3) {
                fprintf(stdout, "\b\b\b   \b\b\b");
                count_period = 0;
            }

            fprintf(stdout, ".");

            ++count_period;
            fflush(stdout);
        }

        throttle(3);
    }
}

char *get_ipaddr(int fd, char *buffer) {
    socklen_t len = 0;
    struct sockaddr_in sa;
    int result = 0;

    len = sizeof sa;
    result = getpeername(fd, (struct sockaddr *)(&sa), &len);

    return (result == 0) ? strcpy(buffer, inet_ntoa(sa.sin_addr)) : NULL;
}

uint16_t get_portno(int fd) {
    socklen_t len = 0;
    struct sockaddr_in sa;
    int result = 0;

    len = sizeof sa;
    result = getpeername(fd, (struct sockaddr *)(&sa), &len);

    return (result == 0) ? ntohs(sa.sin_port) : 0;
}

char *statcode_str(int statcode_num) {
    char *result = NULL;

    switch (statcode_num) {
        case OK_:
        result = "OK!";
        break;

        case EXIST:
        result = "EXIST";
        break;

        case NEXST:
        result = "NEXST";
        break;

        case OPEND:
        result = "OPEND";
        break;

        case EMPTY:
        result = "EMPTY";
        break;

        case NOOPN:
        result = "NOOPN";
        break;

        case NOTMT:
        result = "NOTMT";
        break;

        case WHAT_:
        
        default:
        result = "WHAT?";

        break;
    }

    return result;
}

/**
 *  @brief  TODO
 * 
 *  @param[in]      sec
 */
void throttle(int sec) {
    if (sleep(sec) != 0) {
        throttle(sec);
    }
}
