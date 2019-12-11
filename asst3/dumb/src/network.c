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
#include <strings.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

const char *cmd_engl[] = { "start", "quit",   "create", "open", "next",
                           "put",   "delete", "close",  "help" };

const char *cmd_dumb[] = { "HELLO", "GDBYE", "CREAT", "OPNBX", "NXTMG",
                           "PUTMG", "DELBX", "CLSBX", "USAGE" };

const char *statcode[] = { "OK!",   "EXIST", "NEXST", "OPEND",
                           "EMPTY", "NOOPN", "NOTMT", "WHAT?" };

const char *arg_prompt[] = { "", "", "Okay, what would you like to name your "
                                     "message box?",
                             "Okay, open which message box?", "",
                             "Okay, what would you like your message to say?",
                             "Okay, delete which message box?", "", "", };

int ssocket_open(int domain, int type, uint16_t portno, int backlog) {
    int ssockfd = -1;

    int optval = 1;
    int status = -1;

    struct sockaddr_in server;

    ssockfd = socket(domain, type, 0);

    if (ssockfd == -1) {
        fprintf(stderr, "Unable to create server socket\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(ssockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
        fprintf(stderr, "Unable to set socket options\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *)(&server), sizeof server);

    server.sin_family = domain;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(portno);

    status = bind(ssockfd, (struct sockaddr *)(&server), sizeof server);

    if (status != 0) {
        fprintf(stderr, "Unable to bind server socket\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Now listening on port %d...\n\n", portno);
    fflush(stdout);

    status = listen(ssockfd, backlog);

    if (status != 0) {
        fprintf(stderr, "Unable to listen on port %d\n", portno);
        exit(EXIT_FAILURE);
    }

    return ssockfd;
}

int ssocket_close(int ssockfd) {
    int status = 1;

    if (close(ssockfd) == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    } else {
        fprintf(stdout, "Server socket successfully closed\n");
    }

    status = errno;
    return status;
}

int csocket_open(int domain, int type, const char *hostname, uint16_t portno) {
    int csockfd = -1;

    struct sockaddr_in addr_server;
    struct hostent *server = NULL;

    int status = -1;
    int count_period = 3;

    fprintf(stdout, "Attempting to connect to %s via port %d\n", hostname, portno);

    while (true) {
        status = (csockfd = socket(domain, type, 0));

        if (status < 0) {
            fprintf(stderr, "Error: Socket failed\n");
            exit(EXIT_FAILURE);
        }

        server = gethostbyname(hostname);

        if (server == NULL) {
            fprintf(stderr, "Error: No hostname by identifier %s\n", hostname);
            /* use h_errno */
            exit(EXIT_FAILURE);
        }

        bzero((char *)(&addr_server), sizeof addr_server);

        addr_server.sin_family = domain;

        bcopy((char *)(server->h_addr_list[0]),
              (char *)(&addr_server.sin_addr.s_addr),
              server->h_length);
        addr_server.sin_port = htons(portno);

        status = connect(csockfd, (struct sockaddr *)(&addr_server), sizeof addr_server);

        if (status == 0) {
            fprintf(stdout, "\nConnected (%s via port %d)\n", server->h_name, portno);
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

        throttle(1);
    }

    return csockfd;
}

int csocket_close(int ssockfd) {
    int status = 1;

    if (close(ssockfd) == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    } else {
        fprintf(stdout, "Client socket successfully closed\n");
    }

    status = errno;
    return status;
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

dumbcmd_t cmdarg_capture(char *bufdst) {
    char bufcmd[256];

    int i = 0;
    bool found = false;
    const char *cmd = NULL;

    bzero(bufcmd, 256);

    read(STDIN_FILENO, bufcmd, 256);
    str_trim(bufcmd, "\n");

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

            sprintf(bufdst, "%s %s", cmd, bufarg);
        } else {
            sprintf(bufdst, "%s", cmd);
        }
    } else {
        sprintf(bufdst, "%s", bufcmd);
    }

    return found ? i : ERROR_CODENO;
}

/**
 *  @brief  Takes the contents of bufcmd and bufarg, resulting from
 *          client input, and converts/combines them into a single
 *          string, in DUMB protocol format, stored in bufdst
 *
 *  @param[out] bufdst  buffer to store DUMB protocol message to server
 *  @param[in]  bufcmd  string that consists of a client-typed command

 *  @param[in]  bufarg  string that consists of an argument associated
 *                      with bufcmd, if applicable
 *
 *  @return bufdst, a DUMB protocol formatted message
 *          ex.
 *              bufcmd is "open"
 *              bufarg is "blurp"
 *              bufdst will be "OPNBX blurp"
 *
 *          A malformed message will still result in a readable string.
 */
char *cmdarg_toserv(char *bufdst, char *bufcmd, char *bufarg) {
    int i = 0;
    const char *cmd = NULL;

    bool found = false;

    for (i = 0; i < CMD_COUNT; i++) {
        if (strcmp(bufcmd, cmd_engl[i]) == 0) {
            found = true;
            break;
        }
    }

    if (found) {
        cmd = cmd_dumb[i];

        if (bufarg[0] != '\0') {
            if (strcmp(cmd, cmd_dumb[PUTMG_CODENO]) == 0) {
                sprintf(bufdst, "%s!%lu!%s", cmd, strlen(bufarg), bufarg);
            } else {
                sprintf(bufdst, "%s %s", cmd, bufarg);
            }
        } else {
            sprintf(bufdst, "%s", cmd);
        }

        return bufdst;
    } else {
        if (bufarg[0] != '\0') {
            sprintf(bufdst, "%s %s", bufcmd, bufarg);
        } else {
            sprintf(bufdst, "%s", bufcmd);
        }

        return NULL;
    }
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

 *  @param[out] arg_addr    address of a pointer that will represent
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
int cmdarg_interpret(char *bufsrc, char **arg_addr, ssize_t *arglen_addr) {
    enum cmddumb code = ERROR_CODENO;
    int i = 0;
    bool found = false;

    for (i = 0; i < CMD_COUNT; i++) {
        if (strncmp(bufsrc, cmd_dumb[i], 5) == 0) {
            found = true;
            code = i;
            break;
        }
    }

    if (found) {

    } else {
        
    }

    return code;
}

char *cmdarg_parse(char *bufdst, char *bufsrc) {

    if (strlen(bufsrc) >= 5) {
        if (strncmp(bufsrc, "HELLO", 5) == 0) {

        } else if (strncmp(bufsrc, "GDBYE", 5) == 0) {

        } else if (strncmp(bufsrc, "CREAT", 5) == 0) {

        } else if (strncmp(bufsrc, "OPNBX", 5) == 0) {

        } else if (strncmp(bufsrc, "NXTMG", 5) == 0) {

        } else if (strncmp(bufsrc, "PUTMG", 5) == 0) {
            strcpy(bufdst, bufsrc);
        } else if (strncmp(bufsrc, "DELBX", 5) == 0) {

        } else if (strncmp(bufsrc, "CLSBX", 5) == 0) {

        } else if (strncmp(bufsrc, "USAGE", 5) == 0) {

        } else {
        }
    }

    return bufdst;
}

char *datetime_format(char *bufdst) {
    enum month {
        JAN,
        FEB,
        MAR,
        APR,
        MAY,
        JUN,
        JUL,
        AUG,
        SEP,
        OCT,
        NOV,
        DEC
    };

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

    sprintf(bufdst, "%02d%02d %02d %s", local->tm_hour, local->tm_min, local->tm_mday, month_str);

    return bufdst;
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
