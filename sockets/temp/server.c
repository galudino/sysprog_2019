/**
 *  @file       server.c
 *  @brief      Source file
 *
 *  @author     Gemuele Aludino
 *  @date       26 Nov 2018
 *  @copyright  Copyright © 2018 Gemuele Aludino. All rights reserved.
 */

#include "server.h"
#include "account.h"

int main(int argc, const char *argv[]) {
    /*
     *  User needs to pass in the port number on which the server will accept
     *  connections as an argument. This code displays an error message if
     *  the user fails to do this.
     */
    const bool invalid_arg_count = argc != ARG_COUNT_SERVER;
    
    if (invalid_arg_count) {
        fprintf(stderr, "%s\n"
                "%s\n\n", 
                LBL_ERROR, 
                ERR_INVALID_ARGS_SERVER);
        fprintf(stderr, 
                "USAGE:\n------\n"
                "%s %s\n\n", 
                argv[0], 
                ARGV_1_SERVER);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&accounts_lock, NULL);
    for (int i = 0; i < ACCOUNTS_SIZE; i++) {
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
    
    uint16_t port_number = atoi(argv[1]);
    server_connect(port_number);

    for (int i = 0; i < ACCOUNTS_SIZE; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return EXIT_SUCCESS;
}

void server_connect(uint16_t port_number) {
    int sockfd;

    struct sockaddr_in server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd == -1) {
        fprintf(stderr, 
                "%s\n"
                "%s\n", 
                LBL_ERROR, 
                ERR_FAILED_SOCKET_SERVER);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &server, sizeof (server)) < 0) {        
        fprintf(stderr, 
                "%s\n"
                "%s\n", 
                LBL_ERROR, 
                ERR_FAILED_BIND_SERVER);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, 
            "\n"
            ""KYEL"""[PLEASE WAIT]\n"""KNRM""
            "Now listening on port %d...\n", 
            port_number);
    fflush(stdout);
    
    listen(sockfd, 3);
    
    pthread_t pt_server_handler;
    pthread_t pt_signal_handler;

    if (pthread_create(&pt_server_handler, NULL, server_handler, 
                       (void *) &sockfd) < 0) {
        fprintf(stderr, 
                "%s\n"
                "%s\n", 
                LBL_ERROR, 
                ERR_FAILED_SVRHNDR_SERVER);
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&pt_signal_handler, NULL, signal_handler, NULL) < 0) {
        fprintf(stderr, 
                "%s\n"
                "%s\n", 
                LBL_ERROR, 
                ERR_FAILED_SIGHNDR_SERVER);
        exit(EXIT_FAILURE);
    }

    pthread_join(pt_server_handler, NULL);
}

void *server_handler(void *arg) {
    struct sockaddr_in client;
    int client_addr_len = sizeof (struct sockaddr_in);

    int accept_sockfd;
    int sockfd = *((int *) arg);
    ssockinf_t *ssockinf = NULL;
    
    while ((accept_sockfd = accept(sockfd, (struct sockaddr *) &client, 
                                 (socklen_t *) &client_addr_len))) {
        ssockinf = malloc(sizeof (ssockinf_t));
        
        if (ssockinf == NULL) {
            fprintf(stderr, "%s\n%s\n", LBL_ERROR, ERR_MALLOC_SSOCKINF_T);
            exit(EXIT_FAILURE);
        }
        
        ssockinf->ip_address = inet_ntoa(client.sin_addr);
        ssockinf->port_number = ntohs(client.sin_port);
        
        fprintf(stdout, 
                "\n"
                "%s\n"
                "(%s:%d)\n", 
                LBL_CONNECTED, 
                ssockinf->ip_address,
                ssockinf->port_number);

        ssockinf->new_sockfd = malloc(1);
        memcpy(ssockinf->new_sockfd, &accept_sockfd, sizeof(accept_sockfd));
        
        pthread_t pt_client_handler;
        
        if (pthread_create(&pt_client_handler, NULL, client_handler, 
                           (void *) ssockinf) < 0) {            
            fprintf(stderr, 
                    "%s\n"
                    "%s\n", 
                    LBL_ERROR, 
                    ERR_FAILED_CLIHNDR_SERVER);
        }
    }

    if (accept_sockfd < 0) {        
        fprintf(stderr, "%s\n"
                "%s from address %s:%d\n",
                LBL_ERROR, 
                ERR_FAILED_ACCEPT_SERVER, 
                ssockinf->ip_address,
                ssockinf->port_number);
    }

    return NULL;
}

void *client_handler(void *arg) {
    ssockinf_t *ssockinf = (ssockinf_t *) arg;

    int sockfd = *(ssockinf->new_sockfd);
    int read_size;
    
    char client_message[BUFFER_SIZE];

    while ((read_size = recv(sockfd, client_message, BUFFER_SIZE, 0)) > 0) {
        fflush(stdout);
        
        const bool cmd_create = strcmp(client_message, CMD_CREATE) == 0;
        const bool cmd_serve = strcmp(client_message, CMD_SERVE) == 0;
        const bool cmd_quit = strcmp(client_message, CMD_QUIT) == 0;
        
        if (cmd_create) {
            account_create(sockfd);
        } else if (cmd_serve) {
            account_serve(sockfd);
        } else if (cmd_quit) {
            char *quit = strdup(KYEL"""[QUIT]\n"""KNRM""""QUIT_DISCONNECT);
            const size_t quit_size = strlen(quit) + 1;
            
            write(sockfd, quit, quit_size);
            free(quit);
            quit = NULL;
            
            break;
        }
        
        memset(client_message, 0, strlen(client_message) + 1);
    }
    
    if (read_size >= 0) {
        fprintf(stderr, 
                "\n"
                "%s\n"
                "(%s:%d)\n", 
                LBL_DISCONNECTED, 
                ssockinf->ip_address, 
                ssockinf->port_number);
        fflush(stdout);
    } else if (read_size == -1) {
        fprintf(stderr, 
                "%s\n"
                "%s\n", 
                LBL_ERROR, 
                ERR_FAILED_HANDSHAKE_SERVER);
    }

    if (ssockinf != NULL) {
        free(ssockinf);
        ssockinf = NULL;
    }
    
    pthread_exit(0);
}

void *signal_handler(void *arg) {
    signal(SIGALRM, print_all_accounts);

    alarm(20);

    while (true) {
        pause();
    }

    pthread_exit(0);
}
