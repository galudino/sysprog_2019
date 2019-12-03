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

#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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
