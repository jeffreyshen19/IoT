#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <mraa.h>
#include <include/openssl/bio.h>
#include <include/openssl/ssl.h>
#include <include/openssl/err.h>

static volatile int restart = 1;

void error(const char *msg){
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]){
	int numberIncorrect = 0;
	while (restart) {
		printf("Number incorrect: %d\n", numberIncorrect);

		if (numberIncorrect > 7) {
			printf("More than 7 incorrect guesses: 10 minute wait\n");
			sleep(600);
		}
		else if (numberIncorrect > 5) {
			printf("More than 5 incorrect guesses: 5 minute wait\n");
			sleep(300);
		}
		else if (numberIncorrect > 3) {
			printf("More than 3 incorrect guesses: 1 minutes wait\n");
			sleep(60);
		}
		int client_socket_fd, portno, n;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		printf("1\n");

		// Read command line arguments, need to get the host IP address and port
		if (argc < 3) {
			fprintf(stderr,"usage %s hostname port\n", argv[0]);
			exit(0);
		}

		// Convert the arguments to the appropriate data types
		portno = atoi(argv[2]);

		// setup the socket
		client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		// check if the socket was created successfully. If it wasnt, display an error and exit
		if(client_socket_fd < 0) {
			error("ERROR opening socket");
		}

		// check if the IP entered by the user is valid
		server = gethostbyname(argv[1]);
		if (server == NULL) {
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}

		// clear our the serv_addr buffer
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		// set up the socket
		serv_addr.sin_family = AF_INET;
		memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
		serv_addr.sin_port = htons(portno);

		// try to connect to the server
		if (connect(client_socket_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
			error("ERROR connecting");
		}



		char buffer[256];
				printf("2\n");


		/*
		*
		* SENDING IN THE PASSWORD HERE
		*
		*/

		mraa_aio_context adc_a0;
		uint16_t adc_value = 0;
		float adc_value_float = 0.0;
		mraa_result_t r = MRAA_SUCCESS;

		adc_a0 = mraa_aio_init(0);

		if (adc_a0 == NULL) {
			return 1;
		}


		int values [128];

		mraa_gpio_context button;
		button = mraa_gpio_init(3);
		mraa_gpio_dir(button, MRAA_GPIO_IN);

				printf("3\n");

		while(mraa_gpio_read(button) == 0){
			usleep(10000);
		}
		printf("4\n");

		while(mraa_gpio_read(button) == 1){
			usleep(10000);
		}
		printf("5\n");

		int loopShouldRun = 1;
		int i = 0;

		while(loopShouldRun){
			int k;
			for(k = 0; k < 1000; k++){
				if(mraa_gpio_read(button) == 1) loopShouldRun = 0;
				usleep(1000);
			}
			if(loopShouldRun){
				printf("taking input now\n");
				values[i] = mraa_aio_read(adc_a0);
				i++;
			}
		}
		printf("6\n");

		memset(buffer, 0 ,256);

		int j;
		char password [i + 1];
		for (j = 0; j < i + 1; j++) {
			if (values[j] > 200) {
				password[j] = '1';
			}
			else {
				password[j] = '0';
			}
		}


		r = mraa_aio_close(adc_a0);

		/*
		*
		* DONE SENDING PASSWORD
		*
		*/

		char array[129];
		char arr[129];
		char groupname[6];
		strcat(groupname, argv[3]);
		memcpy(array, password, i);
		memcpy(arr, groupname, 6);
		arr[6] = '\0';
		array[i] = '\0';
		char msgTxt[28];

		strcpy(msgTxt, "ID = "); //Group7 Password =
		strcat(msgTxt, groupname);
		strcat(msgTxt, " Password = ");
		strcat(msgTxt, array);
		strcat(msgTxt, "\n");

		printf("msg txt = %s", msgTxt);

		// send user input to the server

		n = write(client_socket_fd,msgTxt,strlen(msgTxt));
		// n contains how many bytes were received by the server
		// if n is less than 0, then there was an error
		if (n < 0) {
			error("ERROR writing to socket");
		}

		// clear out the buffer
		memset(buffer, 0, 256);

		// get the response from the server and print it to tt he user
		n = read(client_socket_fd, buffer, 255);
		if (n < 0) {
			error("ERROR reading from socket");
		}

		printf("%s\n",buffer);

		if (strstr(buffer, "NO") == NULL) {  //if the buffer contains no then password was wrong
			restart = 0;
			printf("other option");
			close(client_socket_fd);

		}
		else {
			printf("restarting\n");
			numberIncorrect++;
		}
	}

	// clean up the file descriptors
	return 0;
}

