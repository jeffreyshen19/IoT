/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
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

	printf("1\n");

	int server_socket_fd, client_socket_fd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	//

	//

	// error check command line arguments
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	printf("2\n");

	// setup socket
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0) {
		error("ERROR opening socket");
	}

	printf("3\n");

	// setup server information
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	printf("4\n");

	// bind the socket to an address
	if (bind(server_socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	printf("5\n");

	// listen for incoming connections
	// accept at most 5 connections before refusing them
	listen(server_socket_fd, 5);
	clilen = sizeof(cli_addr);

	printf("6\n");

	// block until there is a new connection. When one is received, accept it
	client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	if (client_socket_fd < 0) {
		error("ERROR on accept");
	}

	printf("7\n");

	FILE* file_ptr = fopen(FILE_NAME, "w");
	file_ptr = fopen(FILE_NAME, "w");

	printf("8\n");

	int stop = 0;
	int off = 0;

	while (keepRunning && (off==0)) {

		// clear the buffer
		memset(buffer, 0, 256);
		if(stop == 0){
			n = read(client_socket_fd, buffer, 255); // read what the client sent to the server and store it in "buffer"
			if (n < 0) {
				error("ERROR reading from socket");
			}


			// print the message to console
			printf("Here is the message: %s\n",buffer);
			printf("Sending weather data to the text file...\n");

			fprintf(file_ptr, "%s", buffer);
	  }
		/*
		*
		*/
		sleep(1);

		struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
		char string[128];



		if( poll(&mypoll, 1, 2000) ) //next step is making the timeout the time in the period
		{
			scanf("%9s", string);
			printf("Read string - %s\n", string);
			printf("Message sending = %s\n", string);
			fprintf(file_ptr, "%s\n", string);
			n = write(client_socket_fd,string,strlen(string));
			if (n < 0) {
				error("ERROR writing to socket");
			}
			if (strcmp(string,"STOP") == 0) {
				stop = 1;
			}
			else if (strcmp(string,"START") == 0) {
				stop = 0;
			}
			else if (strcmp(string,"OFF") == 0) {
				off = 1;
				printf("turning off\n");
			}
		}
		else
		{
			// send an acknowledgement back to the client saying that we received the message
			puts("Read nothing");
			n = write(client_socket_fd, "I got your message", 18);
			if (n < 0) {
				error("ERROR writing to socket");
			}
		}


		/*
		*
		*/




	}

	fclose(file_ptr);

	close(client_socket_fd);
	close(server_socket_fd);
	return 0;
}
