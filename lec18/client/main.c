/**
 *  @file       main.c
 *  @brief      Client source file
 *
 *  @author     Gemuele Aludino
 *  @date       07 Nov 2019
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
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

void fildes();

#define SIZE 8192
#define INCR 16

int main(int argc, const char *argv[]) {
    char buffer[SIZE];
    int fd = 0;
    int i = 0;

    char curr_byte = '\0';
    int status = -1;

    fd = open("./client/main.c", O_RDONLY);
    memset(buffer, '\0', SIZE);

    do {
        status = read(fd, buffer + i, INCR);
        i += status;
    } while (status > 0);

    close(fd);

    printf("%s", buffer);

    return 0;
}

void fildes() {
    int fd = -9999;

    fd = open("./client/main.c", O_RDONLY);
    /*fd = open("_dummy_.c", O_RDONLY);*/

    printf("fd: %d\nerror code: %d\nreason: %s\n\n", fd, errno, strerror(errno));

    switch (errno) {
    case EACCES:
    printf("EACCES");
    break;

    case EDQUOT:
    printf("EDQUOT");
    break;

    case EEXIST:
    printf("EEXIST");
    break;

    case ENOENT:
    printf("ENOENT");
    break;

    default:
    printf("all good");
    break;
    }

    printf("\n");

    close(fd);
}
