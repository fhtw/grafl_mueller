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
#include <string>
#include <string.h>
#include <iostream>
#define BUF 1024

using namespace std;

int main (int argc, char **argv)
{
    int create_socket, portNb, check, size, i;
    char buffer[BUF];
    struct sockaddr_in address;
    string lastCommand;

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
        /*command*/
        size=recv(create_socket,buffer,BUF-1, 0);
        if (size < 0)
        {
            perror("recv error");
            return EXIT_FAILURE;

        }
        buffer[size]= '\0';
        printf("%s",buffer);
        fgets (buffer, BUF, stdin);
        send(create_socket, buffer, strlen(buffer)-1, 0);
        lastCommand = string(buffer);
        /*** SEND ***/
        if(lastCommand == "send")
        {
            check = 0;
            /* Sender, Receiver, Subject */
            for(i = 0; i <= 3; i++)
            {
                size=recv(create_socket,buffer,BUF-1, 0);
                if(size > 0)
                {
                    perror("recv error");
                    return EXIT_FAILURE;
                }
                buffer[size]= '\0';
                printf("%s",buffer);
                if(strncmp(buffer,"ERR",3) == 0)
                {
                    check = -1;
                    break;
                }
                fgets (buffer, BUF, stdin);
                send(create_socket, buffer, strlen (buffer), 0);
            }
            /* Content */
            if (check == 0)
            {
                do
                {
                    fgets (buffer, BUF, stdin);
                    send(create_socket, buffer, strlen (buffer), 0);
                }
                while(strcmp(buffer,".\n\r") != 0);
            }
        }

        /*** LIST ***/
        if(lastCommand == "list")
        {
            /* username */
            size=recv(create_socket,buffer,BUF-1, 0);
            if(size > 0)
            {
                perror("recv error");
                return EXIT_FAILURE;
            }
            buffer[size]= '\0';
            printf("%s",buffer);
            fgets (buffer, BUF, stdin);
            send(create_socket, buffer, strlen (buffer)-1, 0);
            do
            {
                size=recv(create_socket,buffer,BUF-1, 0);
                if(size < 0)
                {
                    perror("recv error");
                    return EXIT_FAILURE;
                }
                buffer[size] = '\0';
                if(strncmp(buffer, "FIN", 3) != 0) printf("%s",buffer);
            }
            while(strncmp(buffer, "FIN", 3) != 0);
        }


        /*** DEL & READ ***/
        if(lastCommand == "del" || lastCommand == "read")
        {
            /* username */
            size=recv(create_socket,buffer,BUF-1, 0);
            if(size > 0)
            {
                perror("recv error");
                return EXIT_FAILURE;
            }
            buffer[size]= '\0';
            printf("%s",buffer);
            fgets (buffer, BUF, stdin);
            send(create_socket, buffer, strlen (buffer)-1, 0);
            /*message-id*/
            size=recv(create_socket,buffer,BUF-1, 0);
            if(size > 0)
            {
                perror("recv error");
                return EXIT_FAILURE;
            }
            buffer[size]= '\0';
            printf("%s",buffer);
            if (strncmp(buffer,"ERR",3) == 0) continue;
            fgets (buffer, BUF, stdin);
            send(create_socket, buffer, strlen (buffer), 0);
            /* response -- OK or ERR */
            size=recv(create_socket,buffer,BUF-1, 0);
            if(size > 0)
            {
                perror("recv error");
                return EXIT_FAILURE;
            }
            buffer[size]= '\0';
            printf("%s",buffer);
            if (strncmp(buffer,"ERR",3) == 0) continue;
            if(lastCommand == "read")
            do
            {
                size=recv(create_socket,buffer,BUF-1, 0);
                if(size > 0)
                {
                    perror("recv error");
                    return EXIT_FAILURE;
                }
                buffer[size]= '\0';
                printf("%s",buffer);
                if(strncmp(buffer,"ERR",3) == 0) break;
            }
            while(strcmp(buffer,"OK\n") != 0);
        }
    }
    while (lastCommand != "quit");
    close (create_socket);
    return EXIT_SUCCESS;
}
