/*
    server.cpp
    BIF 3C1
    Alexander Grafl
    Philipp Müller
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <list>
#include <functional>
#include <iterator>
#include <thread>
#include <mutex>
#include <ldap.h>

#define BUF 1024
#define MAX_CLIENT 256
#define DEBUG 0
#define LDAP_SERVER "ldap.technikum-wien.at:389"


using namespace std;
int handleClientInput(int new_socket, string mailDir, string IP, int port);
int delMail(int id, string path, list<string> messageList);
list<string> getFileList(string mailDirectory, string userName);

mutex acceptMutex, outputMutex;


int main(int argc, char** argv)
{
	/*** SOCKET ***/
    int create_socket, new_socket, clientCount=0;
    socklen_t addrlen;
    char buffer[BUF];
    int size, portNb;
    struct sockaddr_in address, cliaddress;
    string mailDir;
    thread t[MAX_CLIENT]; 
    
    if (argc < 2)
    {
        printf("Wrong Number of Arguments!\nUsage: ./%s Port Directory\n",argv[0]);
        return EXIT_FAILURE;
    }
    /*** Port and Directory ***/
    portNb = atoi(argv[1]);
    mailDir = string(argv[2]);

    /*** creating User Directory***/
    if (mkdir(mailDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
    {
        if(errno != EEXIST)
        {
            perror("Directory couldn't be created");
            return EXIT_FAILURE;
        }
        printf("Directory already exists and will be used.\n");
    }

    create_socket = socket (AF_INET, SOCK_STREAM, 0);

    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons (portNb);

    if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0)
    {
        perror("bind error");
        return EXIT_FAILURE;
    }
    listen (create_socket, 5);

    addrlen = sizeof (struct sockaddr_in);

    while (1)
    {
    
		/***
			ToDo:
			-IMPLEMENT LDAP
		***/
        printf("Waiting for connections...\n");
        
        acceptMutex.lock();
        new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
		
        if (new_socket > 0)
        {
            printf ("Client [%d] connected from %s:%d...\n", clientCount, inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port)); 
            strcpy(buffer,"Welcome to TWMailer!\n");
            send(new_socket, buffer, strlen(buffer),0);            
            t[clientCount] = thread(handleClientInput, new_socket, mailDir, string(inet_ntoa (cliaddress.sin_addr)),ntohs(cliaddress.sin_port));
            clientCount++;
        }
        acceptMutex.unlock();
        
               
    }
    for(int i = 0; i <= clientCount; i++) 
    {
    	if(t[i].joinable()) t[i].join();
    }
    close (create_socket);
    return EXIT_SUCCESS;
}


/***
	TODO:
		-Attachments
		-LDAP
		-ban IP after 3 failed logins
***/

int handleClientInput(int new_socket, string mailDir, string IP, int port)
{
	int size, retCode, recID;
	char buffer[BUF];
	string userPath,userFileName, clientUserName, collision, receiver, sender, subject;
	hash<string> str_hash;
	list<string> mailList;
	
 	/*** LDAP ***/
    LDAP        *ld, *ld2;
    int         rc;
    char        bind_dn[100];
    LDAPMessage *result, *e;
    char *dn;
    int has_value;
	/*** LDAP authentification ***/
	
	//send: "Username: " ; recv: buffer 
	sprintf( bind_dn, "uid=%s,dc=technikum-wien,dc=at", buffer );
    printf( "Connecting as %s...\n", bind_dn );

    if( ldap_initialize( &ld, LDAP_SERVER ) )
    {
        perror( "ldap_initialize" );
        EXIT_FAILURE;
    }

    rc = ldap_simple_bind_s( ld, bind_dn, "ashwin" );
    if( rc != LDAP_SUCCESS )
    {
        fprintf(stderr, "ldap_simple_bind_s: %s\n", ldap_err2string(rc) );
        EXIT_FAILURE;
    }

    printf( "Successful authentication\n" );

   	rc = ldap_search_ext_s(ld, "dc=technikum-wien,dc=at", LDAP_SCOPE_SUBTREE, "sn=Müller", NULL, 0, NULL, NULL, NULL, 0, &result);
    if ( rc != LDAP_SUCCESS ) {
        fprintf(stderr, "ldap_search_ext_s: %s\n", ldap_err2string(rc));
    }

    for ( e = ldap_first_entry( ld, result ); e != NULL; e = ldap_next_entry( ld, e ) ) {
        if ( (dn = ldap_get_dn( ld, e )) != NULL ) {
            printf( "dn: %s\n", dn );
            has_value = ldap_compare_s( ld, dn, "userPassword", "secret" ); 
            switch ( has_value ) { 
                case LDAP_COMPARE_TRUE: 
                    printf( "Works.\n"); 
                    break; 
                case LDAP_COMPARE_FALSE: 
                    printf( "Failed.\n"); 
                    break; 
                default: 
                    ldap_perror( ld, "ldap_compare_s" ); 
                    EXIT_FAILURE;
            } 
            ldap_memfree( dn );
        }
    }

  	/***Second bind***/
	if ( (dn = ldap_get_dn( ld, e )) != NULL ) 
	{
        printf( "dn: %s\n", dn );
        /* rebind */
        ldap_initialize(&ld2, LDAP_SERVER);
        rc = ldap_simple_bind_s(ld2, dn, "secret");
        printf("%d\n", rc);
        if (rc != 0) {
            printf("Failed.\n");
        } else {
            printf("Works.\n");
            ldap_unbind(ld2);
        }
        ldap_memfree( dn );
    }
	
	
	/****************************/
	
	do
    {
        strcpy(buffer,"Please enter your command: ");
        send(new_socket, buffer, strlen(buffer),0);
		if(DEBUG==1) cout << "Buffer@Start:" << buffer << endl;
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size < 0)
        {
            perror("recv error (command)");
            return EXIT_FAILURE;
        }
        else if (size == 0)
        {
        	outputMutex.lock();
            printf("Client closed remote socket\n");
            outputMutex.unlock();
            break;
        }

        buffer[size] = '\0';
        
        outputMutex.lock();
        printf ("Message received from %s:%d : %s\n",IP.c_str(),port, buffer);
        outputMutex.unlock();
        /*** SEND ***/
        if(strncmp (buffer, "send", 4)  == 0)
        {
        	
            /* sender */
            strcpy(buffer, "Sender(max. 8 character): ");
            send(new_socket, buffer, strlen(buffer),0);
            
            if(DEBUG == 1) cout << "Buffer: " << buffer << endl;
            
            size = recv(new_socket,buffer,BUF-1, 0);
            if(size > 0 && size <= 8 && strcmp(buffer,"") != 0)
            {
                buffer[size] = '\0';
                sender = string(buffer);
                strcpy(buffer, "Receiver (max. 8 character): ");
                send(new_socket, buffer, strlen(buffer),0);
                /* receiver */
                size = recv(new_socket,buffer,BUF-1, 0);
                if(size > 0 && size <= 8 && strcmp(buffer,"") != 0)
                {
                    buffer[size] = '\0';
                    receiver = string(buffer);
                    strcpy(buffer, "Subject (max. 80 character): ");
                    send(new_socket, buffer, strlen(buffer),0);

                    /* subject */
                    size = recv(new_socket,buffer,BUF-1, 0);
                    if(size > 0 && size <= 80 && strcmp(buffer,"") != 0)
                    {
                        buffer[size] = '\0';
                        mailList = getFileList(mailDir, receiver);
                        subject = string(buffer);
                        /* content */

                        /* string to hash to string ==> Filename*/
                        userPath = mailDir + "/" + receiver + "/";
                        userFileName = receiver + sender + subject;
                        stringstream ss;
                        ss << str_hash(userFileName);
                        userFileName = ss.str();
                        collision = userFileName;
                        /* check if file exists */
                        int count = 0,check = 0;
                        while(check != 1)
                        {
                            ifstream checkFile(userPath+collision);
                            if(checkFile.fail()) check = 1;
                            else
                            {
                                collision = userFileName;
                                collision  += to_string(++count);
                                checkFile.close();

                            }
                        }

                        ofstream newMessage;
                        newMessage.open(userPath + collision);
                        newMessage << sender << endl;
                        newMessage << receiver << endl;
                        newMessage << subject << endl;

                        strcpy(buffer, "Content (quit with \".\"): ");
                        send(new_socket, buffer, strlen(buffer),0);

                        do
                        {
                            size = recv(new_socket,buffer,BUF-1, 0);
                            if(size > 0)
                            {
                                buffer[size] = '\0';
                                if(strcmp(buffer,".\n") != 0) newMessage << buffer;
                            }
                        }
                        while(strcmp(buffer,".\n") != 0);
                        newMessage.close();
                        strcpy(buffer, "OK\n");
                    }
                    else
                    {
                        strcpy(buffer,"ERR - Please try again with a correct Subject (max. 80 character)!\n");
                    }
                }
                else
                {
                    strcpy(buffer,"ERR - Please try again with a correct Receiver (max. 8 character)!\n");
                }
            }
            else
            {
                strcpy(buffer,"ERR - Please try again with a correct Sender (max. 8 character)!\n");
            }
            send(new_socket, buffer, strlen(buffer),0);
        }
        /*** LIST ***/
        if(strncmp (buffer, "list", 4)  == 0)
        {
            strcpy(buffer,"Username (max. 8 characters): ");
            send(new_socket, buffer, strlen(buffer),0);
            size = recv(new_socket,buffer,BUF-1, 0);
            if(size == -1) perror("RECV:");
            if (size > 0 && size <= 8 && strcmp(buffer,"") != 0)
            {
                buffer[size]= '\0';
                clientUserName = string(buffer);
                
               	if((opendir((mailDir + "/" + clientUserName).c_str())) != NULL)
               	{
                    mailList = getFileList(mailDir, clientUserName);
                    strcpy(buffer, ("Number of messages: " + to_string(mailList.size()) + "\n").c_str());
                    send(new_socket, buffer, strlen(buffer),0);
                    if(mailList.size() > 0)
                    {
                        list<string>::iterator i;
                        chdir((mailDir + "/" + clientUserName).c_str());
                        int mailNumber = 0;
                        for(i=mailList.begin(); i != mailList.end(); ++i)
                        {
                            string line;
                            mailNumber++;
                            ifstream mail(*i);
                            if (mail.is_open())
                            {
                                getline (mail,line);
                                getline (mail,line);
                                getline (mail,line);
                                strcpy(buffer, ("#" + to_string(mailNumber) + ": " + line + "\n").c_str());
                                send(new_socket, buffer, strlen(buffer),0);
                                mail.close();
                            }
                        }
                        chdir("../..");
                	}
                    strcpy(buffer,"FIN");
                    send(new_socket, buffer, strlen(buffer),0);
        		}
        		else
        		{
        			strcpy(buffer,"ERR - No such user yet!\n");
                	send(new_socket, buffer, strlen(buffer),0);
        		}
            }
            else
            {
                strcpy(buffer,"ERR - Please try again with a correct Username!\n");
                send(new_socket, buffer, strlen(buffer),0);
            }
        }
        /*** READ ***/
        if(strncmp (buffer, "read", 4)  == 0)
        {
            strcpy(buffer,"Username (max. 8 characters): ");
            send(new_socket, buffer, strlen(buffer),0);
            size = recv(new_socket,buffer,BUF-1, 0);
            if(size == -1) perror("RECV:");
            if (size > 0 && size <= 8 && strcmp(buffer,"") != 0)
            {
                buffer[size]= '\0';
                clientUserName = string(buffer);
                strcpy(buffer,"Message number: ");
                send(new_socket, buffer, strlen(buffer),0);
                size = recv(new_socket,buffer,BUF-1, 0);
                if (size > 0)
                {
                    buffer[size]= '\0';
                    if(DEBUG==1) cout << "ID: " << buffer << endl; 
                    recID = atoi(buffer);

                    mailList = getFileList(mailDir, clientUserName);
                    if(mailList.size() >= recID)
                    {
                        list<string>::iterator i = next(mailList.begin(), recID-1);
                        string line;

                        chdir((mailDir + "/" + clientUserName).c_str());
                        ifstream mail(*i);
                        if (mail.is_open())
                        {
                            while(getline (mail,line))
                            {
                                strcpy(buffer,(line + "\n").c_str());
                                send(new_socket, buffer, strlen(buffer),0);
                            }
                            mail.close();
                            strcpy(buffer,"OK\n");
                        }
                        else
                        {
                            strcpy(buffer,"ERR\n");
                        }

                        chdir("../..");
                    }
                    else
                    {
                        strcpy(buffer,"ERR - Message number does not exist!\n");
                    }
                }
            }
            else
            {
                strcpy(buffer,"ERR - Please try again with a correct Username!\n");
            }
            send(new_socket, buffer, strlen(buffer),0);
        }
        /*** DELETE ***/
        if(strncmp (buffer, "del", 3)  == 0)
        {
            strcpy(buffer,"Username (max. 8 characters): ");
            send(new_socket, buffer, strlen(buffer),0);
            size = recv(new_socket,buffer,BUF-1, 0);
            if(size == -1) perror("RECV:");
            if (size > 0 && size <= 8 && strcmp (buffer, "") != 0)
            {
                buffer[size]= '\0';
                clientUserName = string(buffer);
                strcpy(buffer,"Message-ID: ");
                send(new_socket, buffer, strlen(buffer),0);
                size = recv(new_socket,buffer,BUF-1, 0);

                if (size > 0)
                {
                    buffer[size]= '\0';
                    recID = atoi(buffer);
                }

                mailList = getFileList(mailDir, clientUserName);
                if ((retCode = delMail(recID,mailDir + "/" + clientUserName,mailList)) == 0)
                {
                    // answer OK
                    strcpy(buffer,"OK\n");
                }
                else
                {
                    // answer ERR
                    strcpy(buffer,"ERR - File not found or can't be deleted\n");
                }
            }
            else
            {
                strcpy(buffer,"ERR - Please try again with a correct Username!\n");
            }
            send(new_socket, buffer, strlen(buffer),0);
        }

    }
    while (strncmp (buffer, "quit", 4)  != 0);
    close (new_socket);	
    return 1;
}

list<string> getFileList(string mailDirectory, string userName)
{
    list<string> fileList;
    string dirPath;
    DIR *dirP;
    struct dirent * entry;

    dirPath = mailDirectory + "/" + userName;

    if (mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
    {
        if(errno != EEXIST)
        {
            perror("Directory couldn't be created");
            exit(EXIT_FAILURE);
        }
        outputMutex.lock();
        printf("Directory already exists and will be used.\n");
        outputMutex.unlock();
    }
    dirP = opendir(dirPath.c_str());
    if(dirP != NULL)
    {
        while ((entry = readdir(dirP)) != NULL)
        {
            if (entry->d_type == DT_REG)
            {
                fileList.push_back(entry->d_name);
            }
        }
        closedir(dirP);
    }
    return fileList;
}

int delMail(int id, string path, list<string> messageList)
{
    string filename;

    if(messageList.size() >= id)
    {
        list<string>::iterator i = next(messageList.begin(), id-1);
        chdir((path).c_str());
		
		if (DEBUG == 1) cout << "Filename:" << filename << endl;
        int ret_code = remove((*i).c_str());
        chdir("../..");
        if (ret_code == 0)
        {
            std::cout << "File was successfully deleted\n";
            return 0;
        }
        else
        {
            std::cerr << "Error during the deletion: " << ret_code << '\n';
            return -1;
        }
    }
    return -1;
}
