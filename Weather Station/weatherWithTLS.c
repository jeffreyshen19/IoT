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
#include <resolv.h>
#include <arpa/inet.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>


void error(const char *msg){
	perror(msg);
	exit(0);
}

int create_socket(char *url_str, BIO *out) {
  int sockfd;
  char hostname[256] = "";
  char    portnum[4] = "5000";
  char      proto[6] = "";
  char      *tmp_ptr = NULL;
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  /* ---------------------------------------------------------- *
   * Remove the final / from url_str, if there is one           *
   * ---------------------------------------------------------- */

  printf("5\n");

  port = atoi(portnum);
  printf("%d\n", port);

printf("6\n");
  if ( (host = gethostbyname(url_str)) == NULL ) {
    BIO_printf(out, "Error: Cannot resolve hostname %s.\n",  hostname);
    abort();
  }

  /* ---------------------------------------------------------- *
   * create the basic TCP socket                                *
   * ---------------------------------------------------------- */
   printf("7\n");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  dest_addr.sin_family=AF_INET;
  dest_addr.sin_port=htons(port);
  //dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);
  inet_aton(url_str, &dest_addr.sin_addr);

  printf("%lu\n", dest_addr.sin_addr.s_addr);

  /* ---------------------------------------------------------- *
   * Zeroing the rest of the struct                             *
   * ---------------------------------------------------------- */
   printf("8\n");
  memset(&(dest_addr.sin_zero), '\0', 8);

  tmp_ptr = inet_ntoa(dest_addr.sin_addr);

  /* ---------------------------------------------------------- *
   * Try to make the host connect here                          *
   * ---------------------------------------------------------- */
   printf("9\n");
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) == -1 ) {
    BIO_printf(out, "Error: Cannot connect to host %s [%s] on port %d.\n",
             hostname, tmp_ptr, port);
  }

  return sockfd;
}

int main(int argc, char *argv[]){
  char*           dest_url = argv[1];
  BIO              *certbio = NULL;
  BIO               *outbio = NULL;
  X509                *cert = NULL;
  X509_NAME       *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;
  int server = 0;
  int ret, i;

  printf("1\n");

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  method = SSLv23_client_method();

  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  printf("2\n");

  ssl = SSL_new(ctx);

  printf("3\n");

  server = create_socket(dest_url, outbio);

  printf("3b\n");
  if(server != 0)
    BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);

  SSL_set_fd(ssl, server);

  if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_url);

    printf("4\n");

  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);

  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");



  /*
    SOCKET STUFF
  */
  /*int client_socket_fd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;*/
	char buffer[256];

  /*if (argc < 3) { //Make sure Hostname and Port are provided
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
	}*/

  printf("\033[1m\033[32mConnected to secure server. Sending data now.\x1B[0m\n");

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

  //Write to server
  int bytes;

  memset(buffer, 0 ,256);
  strcpy(buffer, "timestamp,temperature,light levels\n");
  SSL_write(ssl, buffer, strlen(buffer));			/* encrypt & send message */

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
      SSL_write(ssl, buffer, strlen(buffer));			/* encrypt & send message */

		}

    memset(buffer, 0 ,256);
    bytes = SSL_read(ssl, buffer, sizeof(buffer));	/* get reply & decrypt */
    buffer[bytes] = 0;
    printf("%s", buffer);

		if(strstr(buffer, scaleCommandCelsius) != NULL){
			fahrenheit = 0;
		}
		else if(strstr(buffer, scaleCommandFahrenheit) != NULL){
			fahrenheit = 1;
		}
		else if(strstr(buffer, periodCommand) != NULL){
			period = 7;
		}
		else if(strstr(buffer, stopCommand) != NULL){
			stop = 1;
		}
		else if(strstr(buffer, startCommand) != NULL){
			stop = 0;
		}
		else if(strstr(buffer, shutdownCommand) != NULL){
			printf("\033[1m\033[31m%s\x1b[0m\n","SHUTTING DOWN");
			return 0;
		}


		if(stop == 0) printf("\x1b[34m%s\x1b[0m\n",buffer);




    sleep(period);
    //usleep(100000);
  }

  SSL_free(ssl);
  close(server);
  X509_free(cert);
  SSL_CTX_free(ctx);
  BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);
  return 0;
}
