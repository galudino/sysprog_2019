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

#include "utils.h"
#include <netinet/in.h>

int ssocket_open(int domain, int type, uint16_t port, int backlog);
int ssocket_close(int ssockfd);

int csocket_open(int domain, int type, const char *hostname, uint16_t port);
int csocket_close(int csockfd);

char *ipaddr(int fd, char *buffer);
uint16_t portno(int fd);

/* [0, 9) */
enum statcode {
    _OK_STATNO,
    EXIST_STATNO,
    NEXST_STATNO,
    OPEND_STATNO,
    EMPTY_STATNO,
    NOOPN_STATNO,
    NOTMT_STATNO,
    _WHAT_STATNO,
    BOPEN_STATNO
};

/* [0, 10) */
enum cmddumb {
    HELLO_CODENO,
    GDBYE_CODENO,
    CREAT_CODENO,
    OPNBX_CODENO,
    NXTMG_CODENO,
    PUTMG_CODENO,
    DELBX_CODENO,
    CLSBX_CODENO,
    USAGE_CODENO,
    ERROR_CODENO
};

/* [0, 10) */
enum cmdengl {
    start_ENGLNO,
    quit_ENGLNO,
    create_ENGLNO,
    open_ENGLNO,
    next_ENGLNO,
    put_ENGLNO,
    delete_ENGLNO,
    close_ENGLNO,
    help_ENGLNO,
    error_ENGLNO
};

typedef enum statcode statcode_t;
typedef enum cmddumb dumbcmd_t;
typedef enum cmdengl englcmd_t;

#define CMD_COUNT 9
extern const char *cmd_engl[];
extern const char *cmd_dumb[];
extern const char *arg_prompt[];

#define STAT_COUNT 9
extern const char *statcode[];

dumbcmd_t cmdarg_capture(char *bufdst);
dumbcmd_t cmdarg_interpret(char *bufsrc, char **arg_addr, ssize_t *arglen_addr);

statcode_t statcode_interpret(char *bufsrc);

char *cmd_msg_success(char *bufdst, char *arg0, char *arg1, dumbcmd_t cmd);
char *cmd_msg_failed(char *bufdst, char *arg0, char *arg1, dumbcmd_t cmd, statcode_t stat);

char *datetime_format(char *bufdst);

#endif /* NETWORK_H */
