/**
 *  @file       DUMBserver.c
 *  @brief      Client source file (server) for Asst3:
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "vptr.h"
#include "user.h"
#include "network.h"

static int usr_usage(void);

static int usr_creat(vptr_t *v, int fd);
static int usr_opnbx(vptr_t *v, int fd);
static int usr_delbx(vptr_t *v, int fd, int index);
static int usr_gdbye(vptr_t *v, int fd, int index);

static int usr_opnop(vptr_t *v, int fd, int index);
static int usr_putmg(vptr_t *v, int fd, int index);
static int usr_nxtmg(vptr_t *v, int fd, int index);
static int usr_clsbx(vptr_t *v, int fd, int index);

void temp();

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    user_t *u = user_new("charlie");
    char buffer[256];

    printf("active: %s\n", user_active(u) ? "yes" : "no");
    user_open(u);

    strcpy(buffer, "message 1");
    user_message_put(u, buffer);

    strcpy(buffer, "message 2");
    user_message_put(u, buffer);

    strcpy(buffer, "message 3");
    user_message_put(u, buffer);

    user_print(&u, stdout);
    printf("peek: %s\n", *user_message_peek(u));

    printf("active: %s\n", user_active(u) ? "yes" : "no");
    user_close(u);
    printf("active: %s\n", user_active(u) ? "yes" : "no");

    user_delete(&u);

    return EXIT_SUCCESS;
}

int usr_usage(void) {
    return 0; 
}

int usr_creat(vptr_t *v, int fd) {

    return 0;
}

int usr_opnbx(vptr_t *v, int fd) {
    return 0;
}

int usr_delbx(vptr_t *v, int fd, int index) {
    return 0;
}

int usr_gdbye(vptr_t *v, int fd, int index) {
    return 0;
}

int usr_opnop(vptr_t *v, int fd, int index) {
    return 0;
}

int usr_putmg(vptr_t *v, int fd, int index) {
    return 0;
}

int usr_nxtmg(vptr_t *v, int fd, int index) {
    return 0;
}

int usr_clsbx(vptr_t *v, int fd, int index) {
    return 0;
}


void temp() {
    usr_usage();
    usr_creat(NULL, 0);
    usr_opnbx(NULL, 0);
    usr_delbx(NULL, 0, 0);
    usr_gdbye(NULL, 0, 0);
    usr_opnop(NULL, 0, 0);
    usr_putmg(NULL, 0, 0);
    usr_nxtmg(NULL, 0, 0);
    usr_clsbx(NULL, 0, 0);
}
