/**
 *  @file       test.c
 *  @brief      Test source file (server) for Asst3:
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
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
#include <errno.h>

#include <pthread.h>

void test_parse(void);
void test_readwrite(void);
void test(void);

void print_usage() {

}

#include "network.h"
#include "user.h"

void func() {
    char cmd[256];
    char out[256];

    int j = 0;

    bzero(cmd, 256);
    bzero(out, 256);

    j += sprintf(out + j, "%s ", "==>");
    write(STDOUT_FILENO, out, 256);

    read(STDIN_FILENO, cmd, 256);

    printf("%s\n", cmd);
}

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    /*
    char cmd[256];
    char arg[256];
    char out[256];
    char *status = NULL;

    bzero(cmd, 256);
    bzero(arg, 256);
    bzero(out, 256);

    strcpy(cmd, "open");

    strcpy(arg, "argument_string");

    status = cmdarg_toserv(out, cmd, arg);

    if (status) {
        printf("will send to server: %s\n", out);
    } else {
        printf("will send to server, but will be error: %s\n", out);
    }
    */
    /*
    printf("%s\n", cmd_dumb[0]);

    func();*/

    test_parse();
    


    return 0;
}

void test() {
    char cmd[256];
    char arg[256];
    char out[256];

    int j = 0;

    bzero(cmd, 256);
    bzero(arg, 256);
    bzero(out, 256);

    strcpy(cmd, "open");
    strcpy(arg, "blurp");

    if (strcmp(cmd, "open") == 0) {
        j += sprintf(out + j, "%s ", "OPNBX");
        j += sprintf(out + j, "%s", arg);
    }
    
    printf("len = %d, to send: %s", j, out);
    printf("\n");
}

void test_parse() {
    char buffer[256];
    strcpy(buffer, "PUTMG!5!hello");

    if (strncmp(buffer, "PUTMG", 5) == 0) {
        char *ptr = buffer + 5;
        char ch = ' ';
        ch = ptr[0];
        ptr[0] = '\0';
        printf("cmd: %s\n", buffer);
        ptr[0] = ch;

        if (ptr[0] == '!') {
            char *temp = NULL;
            char ch = ' ';
            int len = -1;

            ++ptr;
            temp = strchr(ptr, '!');
            ch = *(temp);
            *(temp) = '\0';

            len = atoi(ptr);
            *(temp) = ch;

            ptr = temp + 1;
            ptr[len] = '\0';

            printf("len: %d\n", len);
            printf("msg: %s\n", ptr);
        }
    }
}

void test_readwrite(void) {
    char buffer[256];
    
    memset(buffer, '\0', 256);
    strcpy(buffer, "==> ");
    write(STDOUT_FILENO, buffer, 256);

    memset(buffer, '\0', 256);
    read(STDIN_FILENO, buffer, 256);
    printf("You wrote: %s\n", buffer);
}
