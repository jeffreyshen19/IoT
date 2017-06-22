/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
    
    //argc is number of arguments
    
    //port: 5000
	int server_socket_fd, client_socket_fd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
    
    
    //port1: 5001
    int server_socket_fd1, client_socket_fd1, portno1;
    socklen_t clilen1;
    char buffer1[256];
    struct sockaddr_in serv_addr1, cli_addr1;
    int n1;

    
	// setup socket
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0) {	
		error("ERROR opening socket");
	}
    
    //setup socket1
    server_socket_fd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd1 < 0) {
        error("ERROR opening socket1");
    }

    
	// setup server information
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
    
    //setup server1 information
    memset((char*) &serv_addr1, 0, sizeof(serv_addr1));
    portno1 = atoi(argv[2]); //ARGUMENT 2 IS SECOND PORT
    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_addr.s_addr = INADDR_ANY;
    serv_addr1.sin_port = htons(portno1);
    
    

	// bind the socket to an address
	if (bind(server_socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}
    
    //bind the second socket to an address
    if (bind(server_socket_fd1, (struct sockaddr *) &serv_addr1, sizeof(serv_addr1)) < 0) {
        error("ERROR on binding 1");
    }
    
    

	// listen for incoming connections
	// accept at most 5 connections before refusing them
	listen(server_socket_fd, 5);
	clilen = sizeof(cli_addr);

    //listen for incoming connections
    listen(server_socket_fd1, 5);
    clilen1 = sizeof(cli_addr1);
    
    
    
	// block until there is a new connection. When one is received, accept it
	client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	if (client_socket_fd < 0) {
		error("ERROR on accept");
	}
    
    //block until new connex1
    client_socket_fd1 = accept(server_socket_fd1, (struct sockaddr *) &cli_addr1, &clilen1);
    if (client_socket_fd1 < 0) {
        error("ERROR on accept 1");
    }
    
    
    
    
    while (1) {
        
        // clear the buffer
        memset(buffer, 0, 256);
        n = read(client_socket_fd, buffer, 255); // read what the client sent to the server and store it in "buffer"
        if (n < 0) {
            error("ERROR reading from socket");
        }
        
        memset(buffer1, 0, 256);
        n1 = read(client_socket_fd1, buffer1, 255);
        if (n1 < 0) {
            error("ERROR reading from socket");
        }
        
        printf("Here is the message from connection 1: %s\n", buffer1);
        
        // print the message to console
        printf("Here is the message from connection 0: %s\n",buffer);
        
        
       
    printf("msg txt = %s", msgTxt);

    // send user input to the server

	    
	    
	    
        // send an acknowledgement back to the client saying that we received the message
        n = write(client_socket_fd,buffer1,strlen(buffer1)); 
        if (n < 0) {
            error("ERROR writing to socket");
        }
        
        n1 = write(client_socket_fd1, buffer, strlen(buffer));
        if (n1 < 0) {
            error("ERROR writing to socket");
        }
        
    }
	close(client_socket_fd);
	close(server_socket_fd);
    close(client_socket_fd1);
    close(server_socket_fd1);
	return 0; 
}
