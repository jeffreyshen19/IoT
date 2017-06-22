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

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

int create_socket(char[], BIO *, char*, int);

void error(const char *msg){
	perror(msg);
	exit(0);
}

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
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
  	INITIALIZE SSL
  */

  char dest_url[] = "http://r01.cs.ucla.edu";
  BIO *certbio = NULL;
  BIO *outbio = NULL;
  X509 *cert = NULL;
  X509_NAME *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;
  int sslServer = 0;
  int ret, i;

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();
  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);
  method = SSLv23_client_method();

SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	ssl = SSL_new(ctx);

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
	//client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	/*if(client_socket_fd < 0) { // check if the socket was created successfully. If it wasnt, display an error and exit
		error("ERROR opening socket");
	}*/

  portno = atoi(argv[2]); // Convert the arguments to the appropriate data types

  // check if the IP entered by the user is valid
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

  // clear our the serv_addr buffer
	/*memset((char *) &serv_addr, 0, sizeof(serv_addr));
	// set up the socket
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	// try to connect to the server
	if (connect(client_socket_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR connecting");
	}*/

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
  SSL_write(ssl, buffer, strlen(buffer));
  /*write(client_socket_fd,buffer,strlen(buffer));
  memset(buffer, 0, 256);
  read(client_socket_fd, buffer, 255);*/

  //Prepare SSL connection
  sslServer = create_socket(dest_url, outbio, server, portno);
  SSL_set_fd(ssl, sslServer);

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

	  //Write to server
  char buf[1024];
  int bytes;

  SSL_write(ssl, buffer, strlen(buffer));			/* encrypt & send message */
  bytes = SSL_read(ssl, buf, sizeof(buf));	/* get reply & decrypt */
  buf[bytes] = 0;
  printf("\x1b[34m%s\x1b[0m\n", buf);

    sleep(1);
    //usleep(100000);
  }

  //Free all connections.
  SSL_free(ssl);
  close(server);
  X509_free(cert);
  SSL_CTX_free(ctx);

  return 0;
}

int create_socket(char url_str[], BIO *out, char* hostname, int port) {
  int sockfd;
  char      proto[6] = "";
  char      *tmp_ptr = NULL;
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  if(url_str[strlen(url_str)] == '/')
    url_str[strlen(url_str)] = '\0';

  strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

  strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));

  if(strchr(hostname, ':')) {
    tmp_ptr = strchr(hostname, ':');
    /* the last : starts the port number, if avail, i.e. 8443 */
    strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
    *tmp_ptr = '\0';
  }

  if ( (host = gethostbyname(hostname)) == NULL ) {
    BIO_printf(out, "Error: Cannot resolve hostname %s.\n",  hostname);
    abort();
  }

  //Create TCP socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  dest_addr.sin_family=AF_INET;
  dest_addr.sin_port=htons(port);
  dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

  memset(&(dest_addr.sin_zero), '\0', 8);

  tmp_ptr = inet_ntoa(dest_addr.sin_addr);

  //Try connecting here
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) == -1 ) {
    BIO_printf(out, "Error: Cannot connect to host %s [%s] on port %d.\n",
             hostname, tmp_ptr, port);
  }

  return sockfd;
}
