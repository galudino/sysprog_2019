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

#include <netinet/in.h>
#include "utils.h"

typedef struct connection_info conninf_t;
struct connection_info {
    int ssockfd;
    int cconnfd;

    void *arg;
};

int ssocket_open(int domain, int type, uint16_t portno, int backlog);
int ssocket_close(int ssockfd);

int csocket_open(int domain, int type, const char *hostname, uint16_t portno);
int csocket_close(int csockfd);

char *get_ipaddr(int fd, char *buffer);
uint16_t get_portno(int fd);

enum statcode {
    OK_,
    EXIST,
    NEXST,
    OPEND,
    EMPTY,
    NOOPN,
    NOTMT,
    WHAT_
};

enum cmdcode {
    HELLO,
    GDBYE,
    CREAT,
    OPNBX,
    NXTMG,
    PUTMG,
    DELBX,
    CLSBX,
    USAGE,
    ERROR
};

char *statcode_str(int statcode_num);

char *cmdarg_parse(char *bufdst, char *bufsrc);

/**< cmdarg_toserv: takes human input and converts it to a DUMB message */
char *cmdarg_toserv(char *bufdst, char *bufcmd, char *bufarg);

/**< cmdarg_interpret: determines cmd, arg, and arglen from cmdarg_toserv */
int cmdarg_interpret(char *bufsrc, char **arg_addr, ssize_t *arglen_addr);

/**< throttle: briefly halt program */
void throttle(int sec);

#endif /* NETWORK_H */
