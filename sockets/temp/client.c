/**
 *  @file       client.c
 *  @brief      Source file
 *
 *  @author     Gemuele Aludino
 *  @date       26 Nov 2018
 *  @copyright  Copyright © 2018 Gemuele Aludino. All rights reserved.
 */

#include "client.h"
#include "account.h"

static char istrue = ' ';

int main(int argc, const char *argv[]) {
    /*
     *  User needs to pass in the port number on which the server will accept
     *  connections as an argument. This code displays an error message if
     *  the user fails to do this.
     */
    const bool invalid_arg_count = argc != ARG_COUNT_CLIENT;

    if (invalid_arg_count) {
        fprintf(stderr, "%s\n%s\n\n", LBL_ERROR, ERR_INVALID_ARGS_CLIENT);
        fprintf(stderr, "USAGE:\n------\n%s %s %s\n\n",
                argv[0], ARGV_1_CLIENT, ARGV_2_CLIENT);
        exit(EXIT_FAILURE);
    }

    const char *hostname = argv[1];
    int port_number = atoi(argv[2]);

    csockinf_t *csockinf = NULL;
    csockinf = csockinf_t_init();
    csockinf_t_connect(&csockinf, hostname, port_number);

    print_main_menu(stdout);

    if (pthread_create(&csockinf->pt_inbound_handler, NULL, inbound_handler,
                       (void *) csockinf) < 0) {
        fprintf(stderr,
                "%s %s\n",
                LBL_ERROR,
                ERR_FAILED_INBOUND_CLIENT);
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&csockinf->pt_outbound_handler, NULL, outbound_handler,
                       (void *) csockinf) < 0) {
        fprintf(stderr,
                "%s %s\n",
                LBL_ERROR,
                ERR_FAILED_OUTBOUND_CLIENT);
        exit(EXIT_FAILURE);
    }

    pthread_join(csockinf->pt_outbound_handler, NULL);
    csockinf_t_deinit(&csockinf);

    return EXIT_SUCCESS;
}

csockinf_t *csockinf_t_init() {
    csockinf_t *csockinf = NULL;
    csockinf = malloc(sizeof (csockinf_t));

    if (csockinf == NULL) {
        fprintf(stderr, "%s\n%s\n", LBL_ERROR, ERR_MALLOC_CSOCKINF_T);
        exit(EXIT_FAILURE);
    }

    csockinf->command = NULL;
    csockinf->arg = NULL;
    csockinf->inbound_message = NULL;
    csockinf->outbound_message = NULL;

    return csockinf;
}

void csockinf_t_deinit(csockinf_t **csockinf) {
    csockinf_t *c = NULL;
    c = *csockinf;

    if (c->command != NULL) {
        free(c->command);
        c->command = NULL;
    }

    if (c->arg != NULL) {
        free(c->arg);
        c->arg = NULL;
    }

    if (c->inbound_message != NULL) {
        free(c->inbound_message);
        c->inbound_message = NULL;
    }

    if (c->outbound_message != NULL) {
        free(c->outbound_message);
        c->outbound_message = NULL;
    }

    close(c->sockfd);

    free(c);
    c = NULL;
}

void csockinf_t_connect(csockinf_t **c, const char *hostname, int port_number) {
    csockinf_t *csockinf = NULL;
    csockinf = *c;

    int period_count = 3;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    csockinf->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (csockinf->sockfd == -1) {
        fprintf(stderr, "%s\n%s\n", LBL_ERROR, ERR_FAILED_SOCKET_CLIENT);
        exit(EXIT_FAILURE);
    }

    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr,
                "%s\n"
                "%s %s.\n",
                LBL_ERROR,
                ERR_NO_HOSTNAME_CLIENT,
                hostname);
        exit(EXIT_FAILURE);
    }

    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port_number);

    fprintf(stdout,
            "\n"""KYEL"""[PLEASE WAIT]\n"""KNRM""""
            "%s %s via port %d...\n",
            ERR_ATTEMPT_CONN_CLIENT,
            hostname,
            port_number);

    while (true) {
        if (connect(csockinf->sockfd, (struct sockaddr *) &serv_addr,
                    sizeof (serv_addr)) < 0) {
            if (period_count == 3) {
                printf("\b\b\b   \b\b\b");
                period_count = 0;
            }

            printf(".");

            period_count++;
            fflush(stdout);
        } else {
            fprintf(stdout,
                    "\n%s\n"
                    "(%s via port %d)\n",
                    LBL_CONNECTED,
                    hostname,
                    port_number);
            break;
        }

        throttle(3);
    }
}

void *inbound_handler(void *arg) {
    csockinf_t *csockinf = (csockinf_t *) arg;

    csockinf->inbound_message = malloc(BUFFER_SIZE);

    int read_size = 0;

    while ((read_size =
            recv(csockinf->sockfd, csockinf->inbound_message, BUFFER_SIZE, 0)) > 0) {
        if (csockinf->inbound_message[0] == '\n') {
            /*
             *  When client begins a session
             */
            istrue = 'T';
            printf("%s\n", csockinf->inbound_message);
        } else if (*(csockinf->inbound_message) == 'F' && istrue == 'T') {
            /*
             *  When client ends a session
             */
            istrue = 'F';

        } else {
            /*
             *  When client receives a message
             *  (either the result of a command, or error)
             */
            printf("%s\n", csockinf->inbound_message);
        }

        memset(csockinf->inbound_message, 0, strlen(csockinf->inbound_message) + 1);
    }

    if (read_size == 0) {
        pthread_cancel(csockinf->pt_outbound_handler);
        fprintf(stderr, "%s\n%s\n", LBL_DISCONNECTED, ERR_DISCONN_CLIENT);
    }

    pthread_exit(0);
}

void *outbound_handler(void *arg) {
    csockinf_t *csockinf = (csockinf_t *) arg;

    while (true) {
        /*
         *  User enters command and arg here through their stdin.
         */
        char bufcmd[BUFFER_SIZE];
        char bufarg[BUFFER_SIZE];

        printf("\n%s\n", LBL_READY);
        printf("%s", CURSOR);
        scanf("%s%99[^\n]", bufcmd, bufarg);

        printf("\n");

        csockinf->command = strdup(bufcmd);
        csockinf->arg = strdup(bufarg);

        const size_t cmd_size = strlen(bufcmd) + 1;
        const size_t arg_size = strlen(bufarg) + 1;

        const bool cmd_create = strcmp(bufcmd, CMD_CREATE) == 0;
        const bool cmd_serve = strcmp(bufcmd, CMD_SERVE) == 0;
        const bool cmd_query = strcmp(bufcmd, CMD_QUERY) == 0;
        const bool cmd_deposit = strcmp(bufcmd, CMD_DEPOSIT) == 0;
        const bool cmd_withdraw = strcmp(bufcmd, CMD_WITHDRAW) == 0;
        const bool cmd_end = strcmp(bufcmd, CMD_END) == 0;
        const bool cmd_quit = strcmp(bufcmd, CMD_QUIT) == 0;

        if (cmd_create || cmd_serve) {
            printf("\n%s\n\n", LBL_WAIT);
            write(csockinf->sockfd, csockinf->command, cmd_size);
            throttle(1);
            write(csockinf->sockfd, csockinf->arg, arg_size);
        } else if (istrue == 'T') {
            if (cmd_query) {
                printf("\n%s\n\n", LBL_WAIT);
                write(csockinf->sockfd, csockinf->command, cmd_size);
            } else if (cmd_end) {
                istrue = 'F';
                printf("\n%s\n\n", LBL_WAIT);
                write(csockinf->sockfd, csockinf->command, cmd_size);
            } else if (cmd_deposit || cmd_withdraw) {
                printf("\n%s\n\n", LBL_WAIT);
                write(csockinf->sockfd, csockinf->command, cmd_size);
                throttle(1);
                write(csockinf->sockfd, csockinf->arg, arg_size);
            } else {
                printf("%s\n", ERR_BAD_INPUT);
            }
        } else if (cmd_quit) {
            write(csockinf->sockfd, csockinf->command, cmd_size);
            throttle(1);
            break;
        } else {
            printf("%s\n", ERR_BAD_INPUT);
        }

        if (csockinf->command != NULL) {
            free(csockinf->command);
            csockinf->command = NULL;
        }

        if (csockinf->arg != NULL) {
            free(csockinf->arg);
            csockinf->arg = NULL;
        }

        throttle(2);
    }

    pthread_exit(0);
}

void throttle(int sec) {
    if (sleep(sec) != 0) {
        throttle(sec);
    }
}
