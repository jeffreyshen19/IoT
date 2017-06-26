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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define FILE_NAME "text.txt"

void error(const char *msg)
{
	printf("9\n");
	perror(msg);
	exit(1);
}

int create_socket(int port)
{
	printf("5\n");
	int s;
	struct sockaddr_in addr;

	printf("6\n");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("7\n");
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Unable to create socket");
		exit(EXIT_FAILURE);
	}

	printf("8\n");
	if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("Unable to bind");
		exit(EXIT_FAILURE);
	}

	if (listen(s, 1) < 0) {
		perror("Unable to listen");
		exit(EXIT_FAILURE);
	}

	return s;
}

void init_openssl()
{
	printf("2\n");
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

void Servlet(SSL* ssl)	/* Serve the connection -- threadable */
{   char buf[1024];
    char reply[1024];
    int sd, bytes;
    const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";

    if ( SSL_accept(ssl) == FAIL )					/* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl);								/* get any certificates */
        bytes = SSL_read(ssl, buf, sizeof(buf));	/* get request */
        if ( bytes > 0 )
        {
            buf[bytes] = 0;
            printf("Client msg: \"%s\"\n", buf);
            sprintf(reply, HTMLecho, buf);			/* construct reply */
            SSL_write(ssl, reply, strlen(reply));	/* send reply */
        }
        else
            ERR_print_errors_fp(stderr);
    }
    sd = SSL_get_fd(ssl);							/* get socket connection */
    SSL_free(ssl);									/* release SSL state */
    close(sd);										/* close connection */
}


void cleanup_openssl()
{
	printf("4\n");
	EVP_cleanup();
}

SSL_CTX *create_context()
{
	printf("3\n");
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	method = SSLv23_server_method();

	ctx = SSL_CTX_new(method);
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	return ctx;
}

void configure_context(SSL_CTX *ctx)
{

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
}

static volatile int keepRunning = 1;

void intHandler(int dummy) {
	keepRunning = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, intHandler);

	printf("10\n");

	int server_socket_fd, client_socket_fd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	printf("11\n");
	//
	int sock;
	SSL_CTX *ctx;
	init_openssl();
	ctx = create_context();
	printf("12\n");
	configure_context(ctx);

	sock = create_socket(4433);
	printf("13\n");
	/* Handle connections */

	int stop = 0;
	int off = 0;

	printf("14\n");
	struct sockaddr_in addr;
	uint len = sizeof(addr);
	SSL *ssl;
	const char reply[] = "test\n";
	
	FILE* file_ptr = fopen(FILE_NAME, "w");
	file_ptr = fopen(FILE_NAME, "w");
	printf("15\n");

	while (keepRunning && (off==0)) {
		printf("waiting 1\n");

		/*
		*
		*/

		int client = accept(sock, (struct sockaddr*)&addr, &len);
		if (client < 0) {
			perror("Unable to accept");
			exit(EXIT_FAILURE);
		}
		printf("waiting 2\n");

		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client);
		printf("waiting 3\n");
		
		Servlet(ssl);

		if (SSL_accept(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
		}
		else {
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
			
		}



		/*
		*
		*/



		/*
		*
		*/
		SSL_free(ssl);
		close(client);



	}

	close(sock);
	SSL_CTX_free(ctx);
	cleanup_openssl();
	//^

	fclose(file_ptr);

	close(client_socket_fd);
	close(server_socket_fd);
	return 0;
}
