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

#define BUF 1024


using namespace std;
int delMail(int id, string path, list<string> messageList);
list<string> getFileList(string mailDirectory, string userName);


int main(int argc, char** argv)
{
	int create_socket, new_socket;
    socklen_t addrlen;
    char buffer[BUF];
    string mailDir,userPath,userFileName, clientUserName, collision, receiver, sender, subject;
   	int size, portNb, retCode, recID;
	DIR * dirp;
	list<string> mailList;
	struct dirent * entry;
    struct sockaddr_in address, cliaddress;
    hash<string> str_hash;
   
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
        printf("Waiting for connections...\n");
        new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );

        if (new_socket > 0)
        {
            printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));
            strcpy(buffer,"Welcome to TWMailer!\n");
            send(new_socket, buffer, strlen(buffer),0);         
        }

        do
        {
        	strcpy(buffer,"Please enter your command: ");
        	send(new_socket, buffer, strlen(buffer),0);
        	
            size = recv (new_socket, buffer, BUF-1, 0);
            if( size < 0)
            {
            	perror("recv error");
               	return EXIT_FAILURE;
            }
            else if (size == 0)
            {
				printf("Client closed remote socket\n");
				break;
			}	
			
			buffer[size] = '\0';
			printf ("Message received: %s\n", buffer);
			
			/*** SEND ***/
			if(strncmp (buffer, "send", 4)  == 0)
			{
				/* sender */
				strcpy(buffer, "Sender(max. 8 character): ");
				send(new_socket, buffer, strlen(buffer),0);
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
									newMessage << buffer;		
								}
							}
							while(strncmp(buffer,".",1) != 0);
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
						strcpy(buffer,"FIN");
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
					
					strcpy(buffer,"Messege number: ");
					send(new_socket, buffer, strlen(buffer),0);
					size = recv(new_socket,buffer,BUF-1, 0);
					if (size > 0)
					{
						buffer[size]= '\0';
						recID = atoi(buffer);
					
						mailList = getFileList(mailDir, clientUserName);
						
						if(mailList.size() > 0)
						{
							list<string>::iterator i;
							chdir((mailDir + "/" + clientUserName).c_str());
							int mailNumber = 0;
							
							for(i=mailList.begin(); i != mailList.end(); ++i)
							{
								string line;
								mailNumber++;
								
								if(mailNumber == recID)
								{
		  							ifstream mail(*i);
									if (mail.is_open())
									{
										while(!getline (mail,line))
										{
											strcpy(buffer,line.c_str());
											send(new_socket, buffer, strlen(buffer),0);
										}	
										mail.close();
										strcpy(buffer,"OK\n");
									}
									else
									{
										strcpy(buffer,"ERR\n");
									}
									send(new_socket, buffer, strlen(buffer),0);
									break;
								}
							} 
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
					if ((retCode = delMail(recID,mailDir+clientUserName,mailList)) == 0)
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

    }
    close (create_socket);
    return EXIT_SUCCESS;
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
		printf("Directory already exists and will be used.\n");
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
	
	if(messageList.size() > 0)
	{
		list<string>::iterator i;
		chdir((path).c_str());
		int mailNumber = 0;
		
		for(i=messageList.begin(); i != messageList.end(); ++i)
		{
			cout << "File: " << (*i) << endl;
			mailNumber++;
			
			if(mailNumber == id)
			{
				
				int ret_code = remove((*i).c_str());
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
		} 
	}
	
	
}
