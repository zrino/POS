/* tcp_server.c */
/* simple TCP server */
/* sends and receives text messages until 'quit' */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF 1024
#define PORT 15000
#define QUIT "quit"
#define FILENAME "andromeda.jpeg"
int main (void) {
  int sockfd, connfd;
  socklen_t addrlen;
  char *buffer = malloc (BUF);
  ssize_t size;
  struct sockaddr_in address;
  const int y = 1;
  struct stat file_stat;
  int bytes_sent;
  int bytes_read;
  char file_size[256];
  int filesize;

  printf ("\e[2J");
  
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) > 0)
    printf ("Socket is allocated.\n");
   else {
	perror("socket: allocation failed");
	exit(EXIT_FAILURE);
    }

  setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons (PORT);
  
  if (bind ( sockfd,
             (struct sockaddr *) &address,
             sizeof (address)) != 0) {
    perror( "bind");
    exit(EXIT_FAILURE);
  }
  
  listen (sockfd, 5);
  addrlen = sizeof (struct sockaddr_in);
  
  

  while (1) {
     connfd = accept ( sockfd,
                           (struct sockaddr *) &address,
                           &addrlen );
			bytes_sent = 0;
			bytes_read = 0;
			  
     if (connfd > 0) {
	
	/*int fd = open(FILENAME, O_RDONLY,0677);
  if(fd == -1)	
  {
	perror("File opening");
	exit(EXIT_FAILURE);
  }
  if(fstat(fd,&file_stat) < 0)
  {
	perror("File stat");
	exit(EXIT_FAILURE);
  }	
*/
	printf ("Client (%s) is connected ...\n", inet_ntoa (address.sin_addr));
	//slanje zahtjeva za imenom filea
	char *filenameRequestString = "Upisite naziv trazenog filea:\n";
	char filenameBuffer[256]; 
	int lenFilenameRequest = send(connfd,filenameRequestString,strlen(filenameRequestString),0);
	int filenameStringSize = recv(connfd,filenameBuffer,256,0);
	if(filenameStringSize == 0)
	{
		perror("Nije primljen naziv filea");
		exit(EXIT_FAILURE);
	}
	filenameBuffer[filenameStringSize] = '\0';
	//otvaranje traženog filea
	printf("Ime trazenog filea:'%s'\n",filenameBuffer);
	FILE *dat1 = fopen(filenameBuffer,"rb");
	if (dat1 == NULL)
	{
		perror("Error file");
		exit(EXIT_FAILURE);
	}
	fseek(dat1, 0L, SEEK_END);
	filesize = ftell(dat1);
	fseek(dat1, 0l ,SEEK_SET);
      	
	//slanje veličine filea
	
	sprintf(file_size,"%d",filesize);         
        int len = send (connfd, file_size, sizeof(file_size), 0);
        printf("Veličina filea %d byteova\n",filesize);
        if(len < 0)
	{
		perror("Greška u slanju");
		exit(EXIT_FAILURE);
	}
       //slanje file contenta    
	while(((bytes_read = fread(buffer,1,BUF,dat1)) > 0))
	{
		printf("Učitano %d byteova\n",bytes_read,buffer);
		int offset = 0;
		while(((bytes_sent = send(connfd,buffer + offset, bytes_read,0)) > 0))
		{
			if(bytes_sent > 0)
			{
				printf("Poslano %d byteova\n",bytes_sent);
				offset += bytes_sent;
				bytes_read -= bytes_sent;
			}
		}
	}


	 
       
     close (connfd);
     fclose(dat1);
     printf("Client disconnected.\n");
     }
  }
  
  close (sockfd);
  
  return EXIT_SUCCESS;
}
