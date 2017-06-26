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
  printf("1\n");
	/*
		SSL connection
	*/
	char           dest_url[] = "172.20.10.13";
  BIO              *certbio = NULL;
  BIO               *outbio = NULL;
  X509                *cert = NULL;
  X509_NAME       *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;
  int server = 0;
  int ret, i;

  printf("2\n");

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  printf("3\n");

  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  method = SSLv23_client_method();

  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  printf("4\n");

  ssl = SSL_new(ctx);

  server = create_socket(dest_url, outbio);
  printf("5\n");
  if(server != 0)
    BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);

  /* ---------------------------------------------------------- *
   * Attach the SSL session to the socket descriptor            *
   * ---------------------------------------------------------- */
  SSL_set_fd(ssl, server);

  /* ---------------------------------------------------------- *
   * Try to SSL-connect here, returns 1 for success             *
   * ---------------------------------------------------------- */
  if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_url);

  printf("6\n");

  /* ---------------------------------------------------------- *
   * Get the remote certificate into the X509 structure         *
   * ---------------------------------------------------------- */
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);

  /* ---------------------------------------------------------- *
   * extract various certificate information                    *
   * -----------------------------------------------------------*/
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  /* ---------------------------------------------------------- *
   * display the cert subject here                              *
   * -----------------------------------------------------------*/
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");

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

  printf("7\n");

  while(1){
    R = 1023.0 /((float) mraa_aio_read(thermometer)) -1.0;
    R = R0 * R;
    thermometer_value = 1.0/(log(R/R0)/B+1/298.15)-263.15;

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
    memset(buffer, 0, 256);
    read(client_socket_fd, buffer, 255);
    printf("\x1b[34m%s\x1b[0m\n",buffer);

    sleep(1);
    //usleep(100000);
  }

  return 0;
}
