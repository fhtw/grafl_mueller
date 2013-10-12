/*
    client.cpp
    BIF 3C1
    Alexander Grafl
    Philipp Müller
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024

using namespace std;
int main (int argc, char **argv) 
{
	int create_socket;
	int portNb;
    char buffer[BUF];
    struct sockaddr_in address;
    int size;

    if( argc < 2 )
    {
         printf("Wrong number of Arguments!\nUsage: %s ServerAddress Port\n", argv[0]);
         exit(EXIT_FAILURE);
    }

	portNb = atoi(argv[2]);
	
    if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
         perror("Socket error");
         return EXIT_FAILURE;
    }

    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (portNb);
    inet_aton (argv[1], &address.sin_addr);

    if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
    {
        printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
        size=recv(create_socket,buffer,BUF-1, 0);
        if (size>0)
        {
            buffer[size]= '\0';
            printf("%s",buffer);
        }
    }
    else
    {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    do
    {
        printf ("Send message: ");
        fgets (buffer, BUF, stdin);
        send(create_socket, buffer, strlen (buffer), 0);
       	
       
       	if(strncmp (buffer, "send", 4)  == 0);
       	if(strncmp (buffer, "list", 4)  == 0);
       	if(strncmp (buffer, "read", 4)  == 0);
       	if(strncmp (buffer, "del", 3)  == 0)
 		{
 			/* Instruction */
	        size=recv(create_socket,buffer,BUF-1, 0);
	       	if (size>0)
	        {
	            buffer[size]= '\0';
	            printf("%s",buffer);
	            /* username */
             	fgets (buffer, BUF, stdin);
             	size=recv(create_socket,buffer,BUF-1, 0);
             	
		       	if (size>0 && strncmp(buffer,"ERR",3) != 0)
		        {
		            buffer[size]= '\0';
		            printf("%s",buffer);
		            /* message id */
	             	fgets (buffer, BUF, stdin);
	           		/* response -- OK or ERR */
	           		size=recv(create_socket,buffer,BUF-1, 0);
			        printf("%s",buffer);
		        }		           	
	 		}
	 	}
       	     	
    }
    while (strcmp (buffer, "quit\n") != 0);
    close (create_socket);
    return EXIT_SUCCESS;
}
