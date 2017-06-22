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

void error(const char *msg){
	perror(msg);
	exit(0);
}

void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;
 
    // Extract floating part
    float fpart = n - (float)ipart;
 
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
 
    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
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
  mraa_aio_context thermometer;
  float thermometer_value = 0.0;;
  thermometer = mraa_aio_init(0);
  
  while(1){
    thermometer_value = 1.0/(log(1023.0/mraa_aio_read_float(thermometer)-1.0)/B+1/298.15)-273.15;
    printf("Temperature is: %.5f\n", thermometer_value);
    
    /*
      Transmit data
    */
     
    memset(buffer, 0 ,256);
		ftoa(thermometer_value, buffer, 4)
    n = write(client_socket_fd,buffer,strlen(buffer));
    memset(buffer, 0, 256);
    n = read(client_socket_fd, buffer, 255);
    printf("%s/n",buffer);
    
    sleep(1);
    //usleep(100000);
  }

  return 0;
}
