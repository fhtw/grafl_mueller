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
//#include <message.h>

#define BUF 1024


using namespace std;
int getIdFromLog(char* path);
void sendMail();
void listMail();
int readMail();
int delMail(int id, string path);
list<string> getFileList(string mailDirectory, string userName);


int main(int argc, char** argv)
{
	int create_socket, new_socket;
    socklen_t addrlen;
    char buffer[BUF];
    string mailDir, clientUserName;
   	int size, portNb, retCode, recID,file_count = 0;
	DIR * dirp;
	list<string> mailList;
	struct dirent * entry;
    struct sockaddr_in address, cliaddress;

	int id;
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
	/*
	dirp = opendir(userName); 
	while ((entry = readdir(dirp)) != NULL)
	{
	    if (entry->d_type == DT_REG)
	    { /* If the entry is a regular file 
	         file_count++;
	    }
	}
	closedir(dirp);
	
	if (file_count == 0)
	{
		std::ofstream outfile (logFilename);
		outfile << "id:0";
		outfile.close();
	}
	else
	{
		
		id = getIdFromLog(userName);
		if (id == -1) 
		{
			printf ("Error: couldn't read latest ID!\nPlease check %s/log.txt file\n",userName);
			return EXIT_FAILURE;
		}
	}*/
				
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
            strcpy(buffer,"Welcome to TWMailer, Please enter your command:\n");
            send(new_socket, buffer, strlen(buffer),0);         
        }

        do
        {
        	/* 
        		ToDo:
        		- DELETE
        		- SEND
        		- LIST
        		- READ
        	*/
        	
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
				//sendMail();
			}
			/*** LIST ***/
			if(strncmp (buffer, "list", 4)  == 0)
			{
				strcpy(buffer,"Username (max. 8 characters): ");
				send(new_socket, buffer, strlen(buffer),0);
				size = recv(new_socket,buffer,BUF-1, 0);	
				if(size == -1) perror("RECV:");
				if (size > 0 && size <= 8)
				{
					buffer[size]= '\0';
					clientUserName = string(buffer);
					mailList = getFileList(mailDir, clientUserName);
					strcpy(buffer, ("Number of messages: " + to_string(mailList.size())).c_str());
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
				/*readMail();
				if ((retCode = readMail(id,userName)) == 0)
				{
					// answer OK
					// + content
				}
				else
				{
					// answer ERR
				}*/
			}
			/*** DELETE ***/
			if(strncmp (buffer, "del", 3)  == 0)
			{
				//fflush(buffer);
				printf("send username");
				strcpy(buffer,"Username (max. 8 characters): ");
				send(new_socket, buffer, strlen(buffer),0);
				printf("befor USERAME");
				size = recv(new_socket,buffer,BUF-1, 0);	
				if(size == -1) perror("RECV:");
				if (size > 0 && size <= 8)
				{
					printf("after USERAME");
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
					if ((retCode = delMail(recID,clientUserName)) == 0)
					{
						// answer OK
						strcpy(buffer,"OK");					
					}
					else
					{
						// answer ERR
						strcpy(buffer,"ERR");
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

/*
int getIdFromLog(char* path)
{
	int id;
	std::string filename;
	std::string inputString, token;
	std::string delimiter = ":";
	
	filename.assign(path);
	filename.append("/log.txt");
	//ToDo: cast char* to String --> use as Path --> ?
	//std::ifstream input (strcat(path,"/log.txt");
	std::ifstream input (filename);
	
	if (input.is_open())
	{
		input >> inputString;
		input.close();
		
		token = inputString.substr(3, inputString.find(delimiter));
		std::stringstream str( token );
		if((str >> id).fail())
		{
			return -1;
		}
	}
	return id;
}*/

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

int delMail(int id, string path)
{
	string filename;
	filename.assign(path);
	filename.append("/" +  to_string(id));
	
	std::vector<char> v(filename.begin(), filename.end());
	v.push_back('\0'); 
	char* cFilename = &v[0];
	
	int ret_code = std::remove(cFilename);
    if (ret_code == 0)
    {
        std::cout << "File was successfully deleted\n";
        return 0;
    } 
    else 
    {
        std::cerr << "Error during the deletion: " << ret_code << '\n';
        return 1;
    }
}
