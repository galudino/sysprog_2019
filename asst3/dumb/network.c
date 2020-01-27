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

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const char *cmd_engl[] = { "start", "quit",   "create", "open", "next",
                           "put",   "delete", "close",  "help" };

const char *cmd_dumb[] = { "HELLO", "GDBYE", "CREAT", "OPNBX", "NXTMG",
                           "PUTMG", "DELBX", "CLSBX", "USAGE" };

const char *statcode[] = { "OK!",   "EXIST", "NEXST", "OPEND", "EMPTY",
                           "NOOPN", "NOTMT", "WHAT?", "BOPEN" };

const char *arg_prompt[] = {
    "",
    "",
    "Okay, what would you like to name your "
    "message box?",
    "Okay, open which message box?",
    "",
    "Okay, what would you like your message to say?",
    "Okay, delete which message box?",
    "",
    "",
};

int ssocket_open(int domain, int type, uint16_t port, int backlog) {
    int ssockfd = -1;

    int optval = 1;
    int status = -1;

    struct sockaddr_in server;

    ssockfd = socket(domain, type, 0);

    if (ssockfd == -1) {
        fprintf(stderr, "unable to create server socket\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(ssockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
        fprintf(stderr, "unable to set socket options\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *)(&server), sizeof server);

    server.sin_family = domain;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    status = bind(ssockfd, (struct sockaddr *)(&server), sizeof server);

    if (status != 0) {
        fprintf(stderr, "unable to bind server socket\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "now listening on port %d...\n\n", port);
    fflush(stdout);

    status = listen(ssockfd, backlog);

    if (status != 0) {
        fprintf(stderr, "unable to listen on port %d\n", port);
        exit(EXIT_FAILURE);
    }

    return ssockfd;
}

int ssocket_close(int ssockfd) {
    int status = 1;

    if (close(ssockfd) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    } else {
        fprintf(stdout, "server socket successfully closed\n");
    }

    status = errno;
    return status;
}

int csocket_open(int domain, int type, const char *hostname, uint16_t port) {
    int csockfd = -1;

    struct sockaddr_in addr_server;
    struct hostent *server = NULL;

    int status = -1;
    int count_period = 3;

    fprintf(stdout, "attempting to connect to %s via port %d\n", hostname, port);

    while (true) {
        status = (csockfd = socket(domain, type, 0));

        if (status < 0) {
            fprintf(stderr, "error: Socket failed\n");
            exit(EXIT_FAILURE);
        }

        server = gethostbyname(hostname);

        if (server == NULL) {
            fprintf(stderr, "error: no hostname by identifier %s\n", hostname);
            /* use h_errno */
            exit(EXIT_FAILURE);
        }

        bzero((char *)(&addr_server), sizeof addr_server);

        addr_server.sin_family = domain;

        bcopy((char *)(server->h_addr_list[0]),
              (char *)(&addr_server.sin_addr.s_addr), server->h_length);
        addr_server.sin_port = htons(port);

        status = connect(csockfd, (struct sockaddr *)(&addr_server), sizeof addr_server);

        if (status == 0) {
            fprintf(stdout, "\n[connected (%s) via port %d]\n", server->h_name, port);
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

        sleep(1);
    }

    return csockfd;
}

int csocket_close(int ssockfd) {
    int status = 1;

    if (close(ssockfd) == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    } else {
        fprintf(stdout, "client socket successfully closed\n");
    }

    status = errno;
    return status;
}

char *ipaddr(int fd, char *buffer) {
    socklen_t len = 0;
    struct sockaddr_in sa;
    int result = 0;

    len = sizeof sa;
    result = getpeername(fd, (struct sockaddr *)(&sa), &len);

    return (result == 0) ? strcpy(buffer, inet_ntoa(sa.sin_addr)) : NULL;
}

uint16_t portno(int fd) {
    socklen_t len = 0;
    struct sockaddr_in sa;
    int result = 0;

    len = sizeof sa;
    result = getpeername(fd, (struct sockaddr *)(&sa), &len);

    return (result == 0) ? ntohs(sa.sin_port) : 0;
}

dumbcmd_t cmdarg_capture(char *bufdst) {
    char bufcmd[256];
    char *newln_addr = NULL;

    dumbcmd_t i = 0;
    bool found = false;
    const char *cmd = NULL;

    bzero(bufcmd, 256);

    read(STDIN_FILENO, bufcmd, 256);
    if ((newln_addr = strchr(bufcmd, '\n'))) {
        *(newln_addr) = '\0';
    }

    for (i = 0; i < CMD_COUNT; i++) {
        if (strcmp(bufcmd, cmd_engl[i]) == 0) {
            found = true;
            break;
        }
    }

    if (found) {
        cmd = cmd_dumb[i];

        if (strlen(arg_prompt[i]) > 0) {
            char bufarg[256];

            bzero(bufarg, 256);
            fflush(stdout);
            printf("%s\n%s:> ", arg_prompt[i], cmd_engl[i]);

            fgets(bufarg, 256, stdin);
            if ((newln_addr = strchr(bufarg, '\n'))) {
                *(newln_addr) = '\0';
            }

            if (strcmp(cmd, cmd_dumb[PUTMG_CODENO]) == 0) {
                sprintf(bufdst, "%s!%lu!%s", cmd, strlen(bufarg), bufarg);
            } else {
                sprintf(bufdst, "%s %s", cmd, bufarg);
            }
        } else {
            sprintf(bufdst, "%s", cmd);
        }
    } else {
        sprintf(bufdst, "%s", bufcmd);
    }

    return found ? i : ERROR_CODENO;
}

/**
 *  @brief  Takes the contents of bufsrc (the return value of cmdarg_toserv;
 *          sent from the client and received by the server), and determines
 *          if the first 5 characters of the string are a valid DUMB protocol
 *          command -- and if so, stores the address of the first character
 *          representing the associated argument, as well as the string length
 *          of the argument, if needed.
 *
 *  @param[in]  bufsrc  DUMB protocol message from client

 *  @param[out] arg         pointer that will represent
 *                          the memory location the first character associated
 *                          with the command at the beginning of bufsrc;
 *                          will be NULL if found unnecessary
 *
 *  @param[out] arglen_addr address of a signed long that will represent
 *                          the string length of (*arg_addr), if needed;
 *                          will be NULL if found unnecessary
 *
 *  @return an integer, representing the enumerations of enum cmdcode
 *          0 = HELLO
 *          1 = GDBYE
 *          2 = CREAT
 *          3 = OPNBX
 *          4 = NXTMG
 *          5 = PUTMG
 *          6 = DELBX
 *          7 = CLSBX
 *          8 = USAGE
 *          9 = ERROR
 *          any integer not described by the enumeration above
 *          [INT_MIN, 0) or [9, INT_MAX + 1) is an error.
 */
dumbcmd_t cmdarg_interpret(char *bufsrc, char **arg, ssize_t *arglen_addr) {
    dumbcmd_t code = ERROR_CODENO;
    bool found = false;
    int i = 0;

    for (i = 0; i < CMD_COUNT; i++) {
        if (strncmp(bufsrc, cmd_dumb[i], 5) == 0) {
            found = true;
            code = i;

            break;
        }
    }

    if (found) {
        char *ptr = NULL;
        char ch = ' ';

        /* ptr is address of delimiter, '!' or ' ' */
        ptr = bufsrc + 5;

        /* save delimiter character */
        ch = *(ptr);

        if (ch == '!') {
            /* if delimiter is '!' ... */
            char *end = NULL;

            /* advance ptr one char past delimiter */
            ptr = ptr + 1;

            /* find next occurence of delimiter */
            end = strchr(ptr, '!');

            /* temporarily null-terminate delimiter */
            *(end) = '\0';

            /* save length of message, convert string to integer */
            *(arglen_addr) = atoi(ptr);

            /* restore delimiter character */
            *(end) = '!';

            /* advance end past delimiter */
            end = end + 1;

            /* save base address of message, starting at end */
            *(arg) = end;
        } else if (ch == ' ') {
            /* if delimiter is ' ' ... */

            /* advance ptr one char past delimiter */
            ptr = ptr + 1;

            /* save base address of message, starting at ptr */
            *(arg) = ptr;

            /* for good measure, save length of string *(arg) */
            *(arglen_addr) = strlen(*(arg));
        } else if (ch == '\0') {
            /* if delimiter is ' ' ... */
        } else {
            /* malformed message -
               initial characters may have matched proper commands,
               but expected delimiters were not found */
            code = ERROR_CODENO;
        }
    }

    return code;
}

/**
 *  @brief  Determine the status code (statcode_t) of a client request
 *
 *  @param[in]  bufsrc
 *
 *  @return TODO
 */
statcode_t statcode_interpret(char *bufsrc) {
    statcode_t i = 0;

    for (i = 0; i < STAT_COUNT; i++) {
        if (strcmp(bufsrc, statcode[i]) == 0) {
            break;
        }
    }

    return i == STAT_COUNT ? (STAT_COUNT - 1) : i;
}

char *datetime_format(char *bufdst) {
    enum month { JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

    time_t now;
    struct tm *local = NULL;

    const char *month_str = NULL;

    time(&now);
    local = localtime(&now);

    switch (local->tm_mon) {
    case JAN:
        month_str = "Jan";
        break;

    case FEB:
        month_str = "Feb";
        break;

    case MAR:
        month_str = "Mar";
        break;

    case APR:
        month_str = "Apr";
        break;

    case MAY:
        month_str = "May";
        break;

    case JUN:
        month_str = "Jun";
        break;

    case JUL:
        month_str = "Jul";
        break;

    case AUG:
        month_str = "Aug";
        break;

    case SEP:
        month_str = "Sep";
        break;

    case OCT:
        month_str = "Oct";
        break;

    case NOV:
        month_str = "Nov";
        break;

    case DEC:
        month_str = "Dec";
        break;

    default:
        month_str = "index error";
        break;
    }

    sprintf(bufdst, "%02d%02d %02d %s", local->tm_hour, local->tm_min,
            local->tm_mday, month_str);

    return bufdst;
}
