/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    //int sockfd, newsockfd, portno, clilen;
    //struct sockaddr_in serv_addr;
	//struct sockaddr_in cli_addr;

					// file descriptor for our server socket
					// file descriptor for a client socket
					// server port to connect to
					// utility variable - size of clientAddressInfo below
					// utility variable - for monitoring reading/writing from/to the socket
					// char array to store data going to and coming from the socket

					// Super-special secret C struct that holds address info for building our server socket
					// Super-special secret C struct that holds address info about our client socket



	// If the user didn't enter enough arguments, complain and exit
    if (argc < 2)
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }



	/** If the user gave enough arguments, try to use them to get a port number and address **/

	// convert the text representation of the port number given by the user to an int

	// try to build a socket .. if it doesn't work, complain and exit



	/** We now have the port to build our server socket on .. time to set up the address struct **/

	// zero out the socket address info struct .. always initialize!

	// set the remote port .. translate from a 'normal' int to a super-special 'network-port-int'

	// set a flag to indicate the type of network address we'll be using

	// set a flag to indicate the type of network address we'll be willing to accept connections from



	/** We have an address struct and a socket .. time to build up the server socket **/

    // bind the server socket to a specific local port, so the client has a target to connect to

	// set up the server socket to listen for client connections

	// determine the size of a clientAddressInfo struct

	// block until a client connects, when it does, create a client socket



	/** If we're here, a client tried to connect **/

	// if the connection blew up for some reason, complain and exit

	// zero out the char buffer to receive a client message

	// try to read from the client socket

	// if the read from the client blew up, complain and exit

	// try to write to the client socket

	// if the write to the client below up, complain and exit


	return 0;
}
