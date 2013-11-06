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
#include "dependencies/banList.h"

#define BUF 1024
#define MAX_CLIENT 256
/*** LDAP Config ***/
#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define BIND_USER NULL		/* anonymous bind with user and pw NULL */
#define BIND_PW NULL
/*** Server Options ***/
#define BANTIME 1800	//in seconds 
#define DEBUG 0



using namespace std;
int handleClientInput(int new_socket, string mailDir, string IP, int port, banHandler *banIP);
int delMail(int id, string path, list<string> messageList);
list<string> getFileList(string mailDirectory, string userName);
string checkFileCollision(string userPath, string userFileName, bool hashFlag);
int ldapAuthentification(string user, string pw);

list<string> userLoggedIn;
list<string>::iterator userIter;
mutex outputMutex;


int main(int argc, char** argv)
{
	/*** SOCKET ***/
    int create_socket, new_socket, clientCount=0;
    socklen_t addrlen;
    char buffer[BUF];
    int portNb;
    struct sockaddr_in address, cliaddress;
    string mailDir;
    thread t[MAX_CLIENT]; 
    string banNotification;
    
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
	banHandler *foo = new banHandler(BANTIME);
	userLoggedIn.clear();
	
    while (1)
    {
        printf("Waiting for connections...\n");
        
        new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
		
        if (new_socket > 0)
        {
        	if((foo->checkIP(inet_ntoa (cliaddress.sin_addr))) == 0)
        	{
	            printf ("Client [%d] connected from %s:%d...\n", clientCount, inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port)); 
	            strcpy(buffer,"Welcome to TWMailer!\n");
	            send(new_socket, buffer, strlen(buffer),0);            
	            t[clientCount] = thread(handleClientInput, new_socket, mailDir, string(inet_ntoa (cliaddress.sin_addr)),ntohs(cliaddress.sin_port), foo);
	            clientCount++;
	        }
	        else
	        {
	        	printf ("Client connected from %s:%d but connection is refused. (time left: %lf s.)\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port),foo->checkIP(inet_ntoa (cliaddress.sin_addr))); 
	           	banNotification = "ERR - Your IP is banned for " + to_string(foo->checkIP(inet_ntoa (cliaddress.sin_addr))) + " seconds!\n";
	            strcpy(buffer,banNotification.c_str());
	            send(new_socket, buffer, strlen(buffer),0); 
	            close(new_socket); 
	        }
	        
        }              
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
		-LDAP - DONE
		-ban IP after 3 failed logins - DONE
		-Multi receiver - DONE
		
		-change sender to userName
		-read => download
		-Attachments
		-Attachment multirecv
***/

int handleClientInput(int new_socket, string mailDir, string IP, int port, banHandler *banIP)
{
	int size, retCode, recID, loginCount = 0;
	bool correct;
	char buffer[BUF];
	string userName, userPW;
	string userPath,userFileName, clientUserName, collision, sender, subject, recvToSplit, receiver, recvList;
	list<string> mailList;
	list<string> recvToken;
	list<string>::iterator strIter;
	
 	/*** 
 		Authentification 
 			-3 times wrong => IP temporary banned! (max ban time --> see define)
 	***/
 	banIP->addIP(IP);
 	while(1)
 	{
 		// User 
 		strcpy(buffer,"Username: ");
        send(new_socket, buffer, strlen(buffer),0);
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size <= 0)
        {
            perror("recv error (Username)");
            return EXIT_FAILURE;
        }
        buffer[size]= '\0';
        userName = string(buffer);
        // Password
        strcpy(buffer,"Password: ");
        send(new_socket, buffer, strlen(buffer),0);
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size <= 0)
        {
            perror("recv error (Password)");
            return EXIT_FAILURE;
        }
        buffer[size]= '\0';
        userPW = string(buffer);
        //LDAP
        if(ldapAuthentification(userName, userPW) == 0) break;
        else
        {
    		banIP->checkIP(IP);
        	if(loginCount != 2) 
        	{
        		strcpy(buffer,"\nWrong Password or Username");
        		send(new_socket, buffer, strlen(buffer),0);
        	}
        }
        if((++loginCount) == 3) 
        {
        	strcpy(buffer,"ERR - Your IP got temporarily banned!\n");
        	send(new_socket, buffer, strlen(buffer),0);
        	close (new_socket);	
        	return -1;
        }
 	}
 	for(userIter = userLoggedIn.begin(); userIter != userLoggedIn.end(); ++userIter)
 	{
 		if( (*userIter) == userName)
 		{
 			strcpy(buffer,"ERR - User is already logged in!\n");
        	send(new_socket, buffer, strlen(buffer),0);
        	close (new_socket);	
        	return -1;
 		} 		
 	}
 	userLoggedIn.push_back(userName);
 	
	
	/***********************/
	
	do
    {
        strcpy(buffer,"Please enter your command: ");
        send(new_socket, buffer, strlen(buffer),0);
		if(DEBUG==1) cout << "Buffer@Start:" << buffer << endl;
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size <= 0)
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
        	correct = false;
            /* sender */
            strcpy(buffer, "Sender(max. 8 character): ");
            send(new_socket, buffer, strlen(buffer),0);
            
            if(DEBUG == 1) cout << "Buffer: " << buffer << endl;
            
            size = recv(new_socket,buffer,BUF-1, 0);
            if(size > 0 && size <= 8 && strcmp(buffer,"") != 0)
            {
                buffer[size] = '\0';
                sender = string(buffer);
                /* receiver */
                strcpy(buffer, "Receiver (max. 8 character) split multiple with \",\": ");
                send(new_socket, buffer, strlen(buffer),0);
                
                size = recv(new_socket,buffer,BUF-1, 0);
                if(size > 0 && strcmp(buffer,"") != 0)
                {
                    buffer[size] = '\0';
                    recvList = recvToSplit = string(buffer);
                    correct = true;
                    
                    string delimiter = ",";
					size_t pos = 0;
					
					while ((pos = recvToSplit.find(delimiter)) != string::npos) {
					    recvToken.push_back(recvToSplit.substr(0, pos));
					    recvToSplit.erase(0, pos + delimiter.length());
					}
					recvToken.push_back(recvToSplit);
										
					for(strIter = recvToken.begin(); strIter != recvToken.end(); strIter++)
					{
						if(DEBUG == 1) cout << "RecvToken: " << (*strIter) << std::endl; 
						if((*strIter).size() > 8 || (*strIter).size() == 0) 
						{
							strcpy(buffer,"ERR - Please try again with a correct Receiver (max. 8 character per receiver)!\n");
							correct = false;
						}
					}
					
					
					if(correct)
					{
						receiver = recvToken.front();
						recvToken.pop_front(); //delete 1st element
						
	                    /* subject */
	                    strcpy(buffer, "Subject (max. 80 character): ");
	                    send(new_socket, buffer, strlen(buffer),0);
	                    
	                    size = recv(new_socket,buffer,BUF-1, 0);
	                    if(size > 0 && size <= 80 && strcmp(buffer,"") != 0)
	                    {
	                        buffer[size] = '\0';
	                        mailList = getFileList(mailDir, receiver);
	                        subject = string(buffer);
	                        
	                        /* content */
	                        userPath = mailDir + "/" + receiver + "/";
	                        userFileName = receiver + sender + subject;
	                        
	                        collision = checkFileCollision(userPath,userFileName,true);
	                        ofstream newMessage;
	                        newMessage.open(userPath + collision);
	                        newMessage << sender << endl;
	                        newMessage << recvList << endl;
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
	                        
	                        
	                        string ccFileName, ccUserPath;
	                        
	                        /*** copy files to other receivers ***/
	                        for(strIter = recvToken.begin(); strIter != recvToken.end(); strIter++)
							{
								ccUserPath = mailDir + "/" + (*strIter) + "/";
	                        	ccFileName = (*strIter) + sender + subject;
	                        	mailList = getFileList(mailDir, (*strIter));
	                        	
								ifstream  src(userPath + collision, ios::binary);
								
								ccFileName = checkFileCollision(ccUserPath,ccFileName,true);
								ofstream  dst(ccUserPath + ccFileName, ios::binary);
								dst << src.rdbuf();
							}
	                    }
	                    else
	                    {
	                        strcpy(buffer,"ERR - Please try again with a correct Subject (max. 80 character)!\n");
	                    }
	            	}
                }
            }
            else
            {
                strcpy(buffer,"ERR - Please try again with a correct Sender (max. 8 character)!\n");
            }
            send(new_socket, buffer, strlen(buffer),0);
            recvToken.clear();
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
                            mail.close();mailList = getFileList(mailDir, clientUserName);
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
    
    userLoggedIn.remove(userName);
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

string checkFileCollision(string userPath, string userFileName, bool hashFlag)
{
	hash<string> str_hash;
	string collision;
	int count = 0, check = 0;
	
	if(hashFlag)
	{
		stringstream ss;
    	ss << str_hash(userFileName);
    	userFileName = ss.str();
    }
    collision = userFileName;
    /* check if file exists */
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
    return collision;
}

int ldapAuthentification(string user, string pw)
{
	LDAP *ld;			/* LDAP resource handle */
    LDAPMessage *result, *e;	/* LDAP result handle */
    BerElement *ber;		/* array of attributes */
    char *attribute;
    char **vals;
    string dn;
    int i, rc=0;
    char *attribs[3];		/* attribute array for search */

	string FILTER = "(uid="+ user +")";
	
    attribs[0]=strdup("uid");		/* return uid of entries */
    attribs[1]=NULL;		/* array must be NULL terminated */


    /* setup LDAP connection */
    if ((ld=ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
    {
        perror("ldap_init failed");
        return -1;
    }

    printf("connected to LDAP server %s on port %d\n",LDAP_HOST,LDAP_PORT);

    /* anonymous bind */
    rc = ldap_simple_bind_s(ld,BIND_USER,BIND_PW);

    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
        return -1;
    }
    else
    {
        printf("bind successful\n");
    }

    /* perform ldap search */
    rc = ldap_search_s(ld, SEARCHBASE, SCOPE, FILTER.c_str(), attribs, 0, &result);

    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr,"LDAP search error: %s\n",ldap_err2string(rc));
        return -1;
    }

    for (e = ldap_first_entry(ld, result); e != NULL; e = ldap_next_entry(ld,e))
    {
        dn = string(ldap_get_dn(ld,e));

        // Now print the attributes and values of each found entry
        for (attribute = ldap_first_attribute(ld,e,&ber); attribute!=NULL; attribute = ldap_next_attribute(ld,e,ber))
        {
            if ((vals=ldap_get_values(ld,e,attribute)) != NULL)
            {
                for (i=0; vals[i]!=NULL; i++)
                {
                    printf("\t%s: %s\n",attribute,vals[i]);
                }
                // free memory used to store the values of the attribute /
                ldap_value_free(vals);
            }
            // free memory used to store the attribute
            ldap_memfree(attribute);
        }
        // free memory used to store the value structure
        if (ber != NULL) ber_free(ber,0);

    }

    rc = ldap_simple_bind_s(ld,dn.c_str(),pw.c_str());

    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
        return -1;
    }
 
    /* free memory used for result */
    ldap_msgfree(result);
    free(attribs[0]);
    ldap_unbind(ld);
    return 0;
}