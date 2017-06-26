#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mraa/aio.h>
#include <mraa.h>
#include <math.h>
#include <time.h>

void error(const char *msg){
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]){
  /*
    SOCKET STUFF
  */
  int client_socket_fd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

  if (argc < 3) { //Make sure Hostname and Port are provided
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

  // setup the socket
	client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket_fd < 0) { // check if the socket was created successfully. If it wasnt, display an error and exit
		error("ERROR opening socket");
	}

  portno = atoi(argv[2]); // Convert the arguments to the appropriate data types

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

  printf("\033[1m\033[32mConnected to server. Sending data now.\x1B[0m\n");

  /*
    SENSOR STUFF
  */
  int R0 = 100000;
  int B = 4275;

	//Config values
  int fahrenheit = 1;
	int period = 1;
	int stop = 0;

	char *scaleCommandCelsius = "SCALE=C";
	char *scaleCommandFahrenheit = "SCALE=F";
	char *periodCommand = "PERIOD=";
	char *stopCommand = "STOP";
	char *startCommand = "START";
	char *shutdownCommand = "OFF";

  mraa_init();
  mraa_aio_context thermometer, lightsensor;;
  float R;
  float thermometer_value = 0.0;
  thermometer = mraa_aio_init(0);
  lightsensor = mraa_aio_init(1);
  int light_value = 0;

  memset(buffer, 0 ,256);
  strcpy(buffer, "timestamp,temperature,light levels\n");
  printf("%s", buffer);
  write(client_socket_fd,buffer,strlen(buffer));
  memset(buffer, 0, 256);
  read(client_socket_fd, buffer, 255);

  while(1){
		if(stop == 0){
			R = 1023.0 /((float) mraa_aio_read(thermometer)) -1.0;
	    R = R0 * R;
	    thermometer_value = 1.0/(log(R/R0)/B+1/298.15)-263.15;
			if(fahrenheit) thermometer_value = 9 * thermometer_value / 5 + 32;

	    /*
	      Transmit data
	    */

	    struct timeval tv;
	    gettimeofday(&tv, NULL);

	    unsigned long millis = (unsigned long)(tv.tv_sec) * 1000 + (unsigned long)(tv.tv_usec) / 1000;

	    light_value = mraa_aio_read(lightsensor);

			memset(buffer, 0 ,256);
			sprintf(buffer, "%lu,%.5f,%d\n", millis, thermometer_value, light_value);
			printf("%s", buffer);
			write(client_socket_fd,buffer,strlen(buffer));
		}

		memset(buffer, 0, 256);
		read(client_socket_fd, buffer, 255);

		if(strstr(buffer, scaleCommandCelsius) != NULL){
			fahrenheit = 0;
		}
		else if(strstr(buffer, scaleCommandFahrenheit) != NULL){
			fahrenheit = 1;
		}
		else if(strstr(buffer, periodCommand) != NULL){
			period = buffer[ststr(buffer, periodCommand) + 7] - '0';
		}
		else if(strstr(buffer, stopCommand) != NULL){
			stop = 1;
		}
		else if(strstr(buffer, startCommand) != NULL){
			stop = 0;
		}
		else if(strstr(buffer, shutdownCommand) != NULL){
			return 0;
		}


		printf("\x1b[34m%s\x1b[0m\n",buffer);




    sleep(period);
    //usleep(100000);
  }

  return 0;
}
