/**
 *  @file       DUMBclient.c
 *  @brief      Client source file (client) for Asst3:
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

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "network.h"

/* #define CLIENT_DEBUG_MESSAGES */

#define ENABLE_RECONNECT
/* #undef ENABLE_RECONNECT */

static void *handler_inbound(void *arg);
static void *handler_outbound(void *arg);

static dumbcmd_t last_cmd = ERROR_CODENO;
static statcode_t last_stat = _WHAT_STATNO;

static char current_box[256];
static char request_box[256];
static char message[256];

static bool started = false;

static void help_menu(void);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, const char *argv[]) {
    int status = -1;
    int csockfd = -1;

    const char *hostname = NULL;
    const char *port_str = NULL;

    uint16_t port = 0;

    pthread_t thread_inbound;
    pthread_t thread_outbound;

    pthread_attr_t attr_inbound;
    pthread_attr_t attr_outbound;

    bool *server_disconnected = NULL;

    if (argc < 3) {
        fprintf(stderr, "USAGE: %s [hostname] [port number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    hostname = argv[1];
    port_str = argv[2];

    port = atoi(port_str);

    printf("\n");

#ifdef ENABLE_RECONNECT
connect:
#endif
    csockfd = csocket_open(AF_INET, SOCK_STREAM, hostname, port);

    printf("[CONNECTED] client session started\n\n");
    help_menu();

    pthread_attr_init(&attr_inbound);
    pthread_attr_init(&attr_outbound);

    if ((status = pthread_create(&thread_inbound, &attr_inbound,
                                 handler_inbound, &csockfd)) < 0) {
        fprintf(stderr, "error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    if ((status = pthread_create(&thread_outbound, &attr_outbound,
                                 handler_outbound, &csockfd)) < 0) {
        fprintf(stderr, "error: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&attr_outbound);
    pthread_attr_destroy(&attr_inbound);

    pthread_join(thread_inbound, (void *)(&server_disconnected));

    if ((*server_disconnected)) {
        pthread_cancel(thread_outbound);

#ifdef ENABLE_RECONNECT
    goto connect;
#endif
    } else {
        pthread_join(thread_outbound, NULL);
    }

    /* allocated in handler_inbound */
    free(server_disconnected);
    server_disconnected = NULL;

    csocket_close(csockfd);

    printf("\n[DISCONNECTED] client session ended\n\n");

    return EXIT_SUCCESS;
}

static void help_menu() {
    printf("DUMBclient "
           "v0\n---------------------------------------------------\n");

    printf("start\t\tstart session with DUMB server\n");
    printf("quit\t\tstop session with DUMB server\n");
    printf("create\t\tcreate a new user/message box on the DUMB server\n");
    printf("open\t\topen an existing user/message box on the DUMB server\n");
    printf(
        "next\t\tget the next message from the currently open message box\n");
    printf("put\t\tput a message into the currently open message box\n");
    printf("delete\t\tdelete a user/message box name on the DUMB server\n");
    printf("close\t\tclose the user/message box name on the DUMB server that "
           "was opened by the client.\n");
    printf("help\t\tsee help menu of all available commands (this menu)\n");

    printf("\nwait for the [ready] ==> indicator before entering input.\n");
}

static void *handler_inbound(void *arg) {
    int fd = *(int *)(arg);

    char buffer_in[256];
    char bufarg[256];
    char *ptr = NULL;

    ssize_t arglen = 0;

    int size_read = -1;
    int i = 0;

    statcode_t stat_reply = _WHAT_STATNO;

    bool *server_disconnected = NULL;

    bzero(buffer_in, 256);
    bzero(bufarg, 256);

    server_disconnected = malloc(sizeof *server_disconnected);
    assert(server_disconnected);

    (*server_disconnected) = false;

    while ((size_read = recv(fd, buffer_in, 256, 0)) > 0) {
        ptr = buffer_in;

#ifdef CLIENT_DEBUG_MESSAGES
        printf("server reply: '%s'\n", buffer_in);
#endif /* CLIENT_DEBUG_MESSAGES */

        for (i = 0; i < STAT_COUNT; i++) {
            size_t buffer_in_len = strlen(buffer_in);

            if (strncmp(buffer_in, statcode[i], 2) == 0) {
                /* first two chars of server reply match a member in statcode */
                if (buffer_in_len == 3 && buffer_in[2] == '!') {
                    /* entirety of server reply is 'OK!' */
                    last_stat = _OK_STATNO;
                    stat_reply = _OK_STATNO;

                    break;
                } else if (buffer_in_len > 2) {
                    /* server reply may be a valid statcode (specialized) */
                    if (buffer_in[2] == '!' && buffer_in[3] != '\0') {
                        /* server reply is 'OK!n', n being a positive integer */
                        last_stat = _OK_STATNO;
                        arglen = atoi(buffer_in + 3);
                        stat_reply = _OK_STATNO;

                        break;
                    } else if (buffer_in_len >= 8 && buffer_in[5] == '!') {
                        /*
                        buffer_in_len >= 8 && buffer_in[5] == '!' means:
                           buffer_in is at least "NXTMG!0!"

                        OLD CODE
                        server reply is a 'NXTMG!n!str', n being a positive integer, str being a string
                        if (strncmp(buffer_in, statcode[i], 5) == 0) {
                            cmdarg_interpret(buffer_in, &ptr, &arglen);
                            last_stat = i;
                            stat_reply = i;

                            break;
                        }
                        END OLD CODE
                        */
                       cmdarg_interpret(buffer_in, &ptr, &arglen);
                       last_stat = i;
                       stat_reply = i;
                       break;
                    }
                }
            } else {
                /* server reply is a statcode error */
                dumbcmd_t cmd = ERROR_CODENO;

                stat_reply = i;
                cmd = cmdarg_interpret(buffer_in, &ptr, &arglen);

                if (cmd == NXTMG_CODENO) {
                    break;
                }

                stat_reply = statcode_interpret(buffer_in);
                break;
            }
        }

#ifdef CLIENT_DEBUG_MESSAGES
        printf("stat_reply (statcode): %s\n", statcode[i]);
#endif /* CLIENT_DEBUG_MESSAGES */

        switch (stat_reply) {
        case _OK_STATNO:
            switch (last_cmd) {
            case HELLO_CODENO:
                if (started) {
                    printf("Error. The client was already started.\n");
                } else {
                    printf("Success! The client has started.\n");
                    started = true;
                }

                break;

            case GDBYE_CODENO:

                break;

            case CREAT_CODENO:
                printf("Success! Message box '%s' was created.\n", request_box);
                bzero(request_box, 256);
                break;

            case OPNBX_CODENO:
                printf("Success! Message box '%s' was opened.\n", request_box);
                strcpy(current_box, request_box);
                bzero(request_box, 256);
                break;

            case NXTMG_CODENO:
                printf("Success! The next message in box '%s' was retrieved, and it says: "
                       "'%s'\n", current_box, ptr);
                break;

            case PUTMG_CODENO:
                printf("Success! Your message, '%s', was pushed to box '%s'.\n", message, current_box);
                bzero(message, 256);
                break;

            case DELBX_CODENO:
                printf("Success! Message box '%s' was deleted.\n", request_box);
                bzero(request_box, 256);
                break;

            case CLSBX_CODENO:
                printf("Success! Message box '%s' was closed.\n", current_box);
                bzero(current_box, 256);
                break;

            case USAGE_CODENO:
                help_menu();
                break;

            case ERROR_CODENO:
                printf("Error. Command was unsuccessful, please try again.\n");
                break;

            default:
                printf("Unspecified error [stat reply: _OK_STATNO], please try again.\n");
                break;
            }

            break;

        case EXIST_STATNO:
            switch (last_cmd) {
            case CREAT_CODENO:
                printf("Error. Message box '%s' already exists.\n", request_box);
                bzero(request_box, 256);
                break;

            default:
                printf("Unspecified error [stat reply: EXIST_STATNO], please try again.\n");
                break;
            }

            break;

        case NEXST_STATNO:
            switch (last_cmd) {
            case OPNBX_CODENO:
            case DELBX_CODENO:
                printf("Error. Message box '%s' does not exist.\n", request_box);
                bzero(request_box, 256);
                break;

            default:
                printf("Unspecified error [stat reply: NEXST_STATNO], please try again.\n");
                break;
            }

            break;

        case OPEND_STATNO:
            switch (last_cmd) {
            case OPNBX_CODENO:
                printf("Error. Message box '%s' is already open.\n", request_box);
                bzero(request_box, 256);
                break;

            case DELBX_CODENO:
                printf("Error. Message box '%s' must be closed and empty "
                       "before attempting to delete it.\n",
                       current_box);
                break;

            default:
                printf("Unspecified error [stat reply: OPEND_STATNO], please try again.\n");
                break;
            }

            break;

        case EMPTY_STATNO:
            switch (last_cmd) {
            case NXTMG_CODENO:
                printf("Error. Message box '%s' is empty.\n", current_box);
                break;

            default:
                printf("Unspecified error [stat reply: EMPTY_STATNO], please try again.\n");
                break;
            }

            break;

        case NOOPN_STATNO:
            switch (last_cmd) {
            case NXTMG_CODENO:
                printf("Error. A message box must be open in order to retrieve "
                       "its messages.\n");
                break;

            case CLSBX_CODENO:
                printf("Error. A message box must be open in order to close "
                       "it.\n");
                break;

            case PUTMG_CODENO:
                printf("Error. A message box must be open in order to put "
                       "messages into it.\n");
                bzero(message, 256);
                break;

            default:
                printf("Unspecified error [stat reply: NOOPN_STATNO], please try again.\n");
                break;
            }

            break;

        case NOTMT_STATNO:
            switch (last_cmd) {
            case DELBX_CODENO:
                printf("Error. Message box '%s' must be closed and empty "
                       "before attempting to delete it.\n",
                       request_box);
                bzero(request_box, 256);
                break;

            default:
                printf("Unspecified error [stat reply: NOTMT_STATNO], please try again.\n");
                break;
            }

            break;

        case _WHAT_STATNO:
            switch (last_cmd) {
            case HELLO_CODENO:
                printf(
                    "Try again.\n\nType 'start' (without quotation marks) and "
                    "hit the RETURN key to initialize the DUMBv0 client.\n");
                break;

            default:
                printf("Error. Command was unsuccessful, please try again.\n");
                break;
            }

            break;

        case BOPEN_STATNO:
            switch (last_cmd) {
            case OPNBX_CODENO:
                printf("Error. Message box '%s' must be closed before opening "
                       "another box.\n",
                       current_box);
                break;

            default:
                printf("Unspecified error [stat reply: BOPEN_STATNO], please try again.\n");
                break;
            }

            break;

        default:
            printf("Unspecified error (unknown command?), please try again.\n");

            break;
        }

        bzero(buffer_in, 256);
        sleep(1);
        printf("almost ready...\n");
    }

    if (size_read == 0) {
        if (last_cmd != GDBYE_CODENO) {
            (*server_disconnected) = true;
            fprintf(stdout,
                    "[CONNECTION SEVERED] server has abruptly left the network\n");
        } else {
            fprintf(stdout, "[DISCONNECTING] user is leaving the network\n");
        }
    }

    pthread_exit(server_disconnected);
}

static void *handler_outbound(void *arg) {
    int fd = *(int *)(arg);

    char buffer_out[256];

    char *ptr = NULL;
    ssize_t len = 0;

    while (last_cmd != GDBYE_CODENO) {
        bzero(buffer_out, 256);

        if (started) {
            strcpy(buffer_out, "\n[ready]\n==> ");
        } else {
            strcpy(buffer_out, "\n[type 'start' and hit RETURN to initialize the "
                               "client.]\n==> ");
        }

        write(STDOUT_FILENO, buffer_out, 256);
        bzero(buffer_out, 256);

        if (started) {
            last_cmd = cmdarg_capture(buffer_out);

            if (buffer_out[5] == ' ') {
                switch (last_cmd) {
                case CREAT_CODENO:
                case OPNBX_CODENO:
                case DELBX_CODENO:
                    cmdarg_interpret(buffer_out, &ptr, &len);
                    strcpy(request_box, buffer_out + 6);

                    break;
                default:
                    strcpy(buffer_out, cmd_dumb[last_cmd]);
                    break;
                }
            } else if (buffer_out[5] == '\0') {
                switch (last_cmd) {
                case CLSBX_CODENO:
                    sprintf(buffer_out + 5, " %s", current_box);
                    break;

                default:
                    strcpy(buffer_out, cmd_dumb[last_cmd]);
                    break;
                }

                cmdarg_interpret(buffer_out, &ptr, &len);
            } else if (buffer_out[5] == '!') {
                cmdarg_interpret(buffer_out, &ptr, &len);

                switch (last_cmd) {
                case PUTMG_CODENO:
                case NXTMG_CODENO:
                    if (ptr != NULL) {
                        strncpy(message, ptr, len);
                    }

                    break;
                default:
                    strcpy(buffer_out, cmd_dumb[last_cmd]);
                    break;
                }
            }
        } else {
            char *newln_addr = NULL;

            read(STDIN_FILENO, buffer_out, 256);
            if ((newln_addr = strchr(buffer_out, '\n'))) {
                *(newln_addr) = '\0';
            }

            if (strcmp(buffer_out, cmd_engl[0]) == 0) {
                bzero(buffer_out, 256);

                strcpy(buffer_out, cmd_dumb[0]);
                last_cmd = HELLO_CODENO;
            } else {
                printf("\nYou must type 'start' and hit RETURN in order to "
                       "proceed.\nPlease try again.\n");
            }
        }

        if (last_cmd == GDBYE_CODENO) {
            printf("\n[PLEASE WAIT]\n");
        } else {
            printf("\n[PLEASE WAIT]\nserver is busy...\nwait for [ready] ==> "
                   "indicator.\n\n");
        }

        sleep(1);

#ifdef CLIENT_DEBUG_MESSAGES
        printf("\nlast_cmd == %s_CODENO\n", cmd_dumb[last_cmd]);
        printf("ptr: %s\nlen: %lu\n", ptr, len);

        printf("\nWill send to server: %s\n\n", buffer_out);
#endif /* CLIENT_DEBUG_MESSAGES */

        write(fd, buffer_out, 256);
        bzero(buffer_out, 256);

        sleep(2);
    }

    pthread_exit(NULL);
}
