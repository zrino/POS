/* tcp_client.c */
/* simple TCP client */
/* receives and sends text messages until 'quit' */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define BUF 1024
#define PORT 15000
#define QUIT "quit"
#define FILENAME "received_file.jpeg"
int main (int argc, char **argv) {
  int sfd;
  char *buffer = malloc (BUF);
  struct sockaddr_in address;
  int size;

  if( argc < 2 ){
     printf("Usage: %s ServerAddress\n", *argv);
     exit(EXIT_FAILURE);
  }

  printf ("\e[2J"); // clear
  
  if ((sfd = socket (AF_INET, SOCK_STREAM, 0)) > 0)
    printf ("Socket is allocated.\n");
  else {
	perror("socket: allocation failed");
	exit(EXIT_FAILURE);
    }

  address.sin_family = AF_INET;
  address.sin_port = htons (PORT);
  inet_aton (argv[1], &address.sin_addr);

  if (connect ( sfd,
                (struct sockaddr *) &address,
                sizeof (address)) == 0)
    printf ("Connection with server (%s) established.\n",inet_ntoa (address.sin_addr));
  else {
 	perror("connect");
	exit(EXIT_FAILURE);
  }
	//posalji naziv filea
	char filenameRequestBuffer[256];
	char filename[256];
	int len = recv(sfd,filenameRequestBuffer,256,0);
	if(len == 0)
	{
		perror("Greška u primanju requesta");
		exit(EXIT_FAILURE);
	}
	filenameRequestBuffer[len] = '\0';
	printf("%s",filenameRequestBuffer);
	scanf("%s",filename);
	len = send(sfd,filename,strlen(filename),0);
	
	//primi velicinu filea
       int file_size,remain_data,written;
      size = recv(sfd, buffer, BUF-1, 0);
      if( size > 0 ) {
	file_size = atoi(buffer);
	remain_data = file_size;
 	printf("Veličina filea %d byteova\n",file_size);
         FILE *dat1 = fopen(FILENAME,"wb");
	if(dat1 == NULL)
	{
		perror("File");
		exit(EXIT_FAILURE);
	}
	
	while(((size = recv(sfd,buffer,BUF,0)) > 0) && (remain_data > 0))
		{
		int n = fwrite(buffer,1,size,dat1);
		if(n == -1)
		{
			perror("Write");
			exit(EXIT_FAILURE);
		}
		remain_data -= size;
		printf("Receive %d bytes and we hope :- %d bytes\n",size,remain_data);
		}
	fclose(dat1);
         }

	
  
  
  close (sfd);
  printf("Disconnected.\n");
  
  return EXIT_SUCCESS;
}
