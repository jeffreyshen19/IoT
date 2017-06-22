/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define FILE_NAME "text.txt"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, intHandler);
	
	int server_socket_fd, client_socket_fd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// error check command line arguments
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	// setup socket
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0) {
		error("ERROR opening socket");
	}

	// setup server information
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// bind the socket to an address
	if (bind(server_socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	// listen for incoming connections
	// accept at most 5 connections before refusing them
	listen(server_socket_fd, 5);
	clilen = sizeof(cli_addr);

	// block until there is a new connection. When one is received, accept it
	client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	if (client_socket_fd < 0) {
		error("ERROR on accept");
	}

	FILE* file_ptr = fopen(FILE_NAME, "w");
	file_ptr = fopen(FILE_NAME, "w");

	while (keepRunning) {

		// clear the buffer
		memset(buffer, 0, 256);
		n = read(client_socket_fd, buffer, 255); // read what the client sent to the server and store it in "buffer"
		if (n < 0) {
			error("ERROR reading from socket");
		}

		// print the message to console
		printf("Here is the message: %s\n",buffer);
		printf("Sending weather data to the text file...\n");

		fprintf(file_ptr, "%s", buffer);



		// send an acknowledgement back to the client saying that we received the message
		n = write(client_socket_fd, "I got your message", 18);
		if (n < 0) {
			error("ERROR writing to socket");
		}
	}

	fclose(file_ptr);
	
	close(client_socket_fd);
	close(server_socket_fd);
	return 0;
}
