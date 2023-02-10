#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>

#define PORT 5050
const int string_length = 1024;


// For Read File
void ReadFile(int clientSocket){
	int len,lines=0;
	char buffer[string_length];

	recv(clientSocket,&lines,sizeof(lines),0);
	bzero(buffer,strlen(buffer));
	// printf("lines %d",lines);
	for(int j =0; j<lines; j++){
		recv(clientSocket,buffer,string_length,0);
		printf("%s\n",buffer);
		bzero(buffer,strlen(buffer));
	}
	// printf("\nFile Uploaded Successfully!!\n");

	send(clientSocket,&lines,sizeof(lines),0);
	bzero(buffer,strlen(buffer));
	fflush(stdin);
}



// Check String is Number or Not
int isString_Number(char *str)
{
    // char s[30];
    int i=0,flag=0,n=0;
    if(str[0] == '-' && str[1] == '0') return flag;
    if(str[0] == '-') i=1;
    for(i ; str[i]!='\0'; i++)
    {
        if(str[i]=='0'||str[i]=='1'||str[i]=='2'||str[i]=='3'||str[i]=='5'||str[i]=='6'||str[i]=='7'||str[i]=='8'||str[i]=='9')
            flag = 1;
        else{
            flag = 0;
            break;
        }     
    }
    return flag;
}



//Download_Upload File from Server Site
void download_ServerFile(char filename[],int clientSocket)
{
	FILE *fp;
	int len,b=0;
	char buffer[string_length];

	fp=fopen(filename,"w");

	if(!fp)
	{
		printf("Error in the file.\n");
		exit(1);
	}
	recv(clientSocket,&b,sizeof(b),0);
	bzero(buffer,sizeof(buffer));

	while(b>0)
	{
		bzero(buffer,sizeof(buffer));
		recv(clientSocket,buffer,sizeof(buffer),0);
		len=strlen(buffer);
		b=b-len;
		fprintf(fp,"%s",buffer);
	}
	send(clientSocket,&b,sizeof(b),0);
	bzero(buffer,sizeof(buffer));
	fclose(fp);
}



//Client File Upload Portion
void uploadFile(char filename[],int clientSocket)
{
	int len,b=0;
	FILE *fp;
	char buffer[string_length];

	fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}
	
	while(fgets(buffer,sizeof(buffer),fp))
	{
		len=strlen(buffer);
		b=b+len;
	}

	send(clientSocket,&b,sizeof(b),0);

	rewind(fp);

	while(fgets(buffer,sizeof(buffer),fp))
	{
		send(clientSocket,buffer,strlen(buffer),0);
		bzero(buffer,sizeof(buffer));
	}
	printf("\nFile Uploaded Successfully!!\n");

	recv(clientSocket,&b,sizeof(b),0);
	bzero(buffer,sizeof(buffer));
	fflush(stdin);
	fclose(fp);
}



//Client command Varification
int checkCommand(char *str)
{
	char firstPart[string_length], secondPart[string_length], thirdPart[string_length], fourthPart[string_length];

	//Iniatialization for File Name Checking
	regex_t regex;
	int reti;
	char msgbuf[100];

    bzero(firstPart, string_length);
	bzero(secondPart, string_length);
	bzero(thirdPart, string_length);
	bzero(fourthPart, string_length);

	sscanf(str, "%s %s %s %s", firstPart, secondPart, thirdPart, fourthPart);

	/* Compile regular expression */
	reti = regcomp(&regex, "^\\w+.(txt)$", REG_EXTENDED | REG_NOSUB);

    if(strcmp("/users",firstPart) == 0 && strlen(secondPart) == 0 && strlen(thirdPart) == 0 && strlen(fourthPart) == 0) return 1;
	else if(strcmp("/files",firstPart) == 0 && strlen(secondPart) == 0 && strlen(thirdPart) == 0 && strlen(fourthPart) == 0) return 2;
	else if(strcmp("/upload",firstPart) == 0 && strlen(thirdPart) == 0 && strlen(fourthPart) == 0)
	{
		/* Execute regular expression */
		reti = regexec(&regex, secondPart, 0, 0, 0);
		if (!reti) {
			return 3;
		}
		regfree(&regex);		
	} 
	else if(strcmp("/download",firstPart) == 0 && strlen(thirdPart) == 0 && strlen(fourthPart) == 0)
	{
		/* Execute regular expression */
		reti = regexec(&regex, secondPart, 0, 0, 0);
		if (!reti) {
			return 4;
		}
		regfree(&regex);		
	} 
	else if(strcmp("/invite",firstPart) == 0 && isString_Number(thirdPart) && (strcmp("V",fourthPart) == 0 || strcmp("E",fourthPart) ==0))
	{
		/* Execute regular expression */
		reti = regexec(&regex, secondPart, 0, 0, 0);
		if (!reti) {
			return 5;
		}
		regfree(&regex);		
	} 
	else if(strcmp("/read",firstPart) == 0)
	{
		/* Execute regular expression */
		reti = regexec(&regex, secondPart, 0, 0, 0);
		if (!reti) {
			return 6;
		}
		regfree(&regex);		
	}
	else if (strcmp("/exit",firstPart)==0 && strlen(secondPart)==0 && strlen(thirdPart)==0 && strlen(fourthPart)==0) return 9;
	else return 0;
}




//Valid Filename Check
int checkFilename(char str[])
{
	regex_t regex;
	int reti;
	char msgbuf[100];

	/* Compile regular expression */
	reti = regcomp(&regex, "^\\w+.(txt)$", REG_EXTENDED | REG_NOSUB);

	/* Execute regular expression */
	reti = regexec(&regex, str, 0, 0, 0);
	if (!reti) {
		return 1;
	}
	regfree(&regex);
	return 0;
}


int main(){

	int clientSocket, ret, pass, total_client, total_file, x,activity;
	struct sockaddr_in serverAddr;
	char buffer[string_length], msg[string_length], cmndStart[string_length];
	char field1[string_length], field2[string_length], field3[string_length], field4[string_length], field5[string_length];
	fd_set readfds;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	// memset(&serverAddr, '\0', sizeof(serverAddr));
	bzero((char *)&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}

	// Limit Validation
	bzero(buffer, string_length);
	if(recv(clientSocket, buffer, string_length, 0) < 0){
		printf("Error in receiving data.\n");
	}
	else if(strcmp(buffer, "limit_cross")==0){
		printf("Server:\tClient Limit Exceed.\n");
		// printf("Limit Exceed.\n");
		close(clientSocket);
		exit(1);
	}
	else{
		printf("Welcome! Successfully Connected to Server.\n");
		printf("Give the Command Perfectly. Formet is given bellow: \n");
		printf("1.\t/users\n2.\t/files\n3.\t/upload <filename>\n4.\t/download <filename>\n");
		printf("5.\t/invite <filename> <client_id> <permission>\n6.\t/read <filename> <start_idx> <end_idx>\n");
		printf("7.\t/insert <filename> <idx> “<message>”\n8.\t/delete <filename> <start_idx> <end_idx>\n9.\t/exit\n");
	}

	while(1){
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(clientSocket, &readfds); //invite
		FD_SET(0, &readfds); //client

		activity = select( clientSocket + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		if (FD_ISSET(clientSocket, &readfds) )
		{
			bzero(buffer, string_length);
			if(recv(clientSocket, buffer, string_length, 0) < 0){
				printf("Error in receiving data.\n");
			}
			else
			{
				char res[200],cmd1[100],cmd2[100],cmd3[100],cmd4[100];
				int owner;
				bzero(cmndStart, string_length);
				bzero(field2, string_length);
				bzero(field3, string_length);
				bzero(field4, string_length);

				//Keep record Command First Part
				sscanf(buffer, "%s %d %s %s", cmndStart, &owner, cmd2, cmd4); 
				
				// if((strcmp(cmd4, "V")==0 || strcmp(cmd4, "E")==0) && isString_Number(cmd1) && checkFilename(cmd2))
				if(strcmp(cmndStart, "invitation")==0 && (strcmp(cmd4, "V")==0 || strcmp(cmd4, "E")==0) && checkFilename(cmd2))
				{
					printf("Cleint %d has invited you as %s of file %s.Will you accept?\n",owner,cmd4,cmd2);
					printf("Enter yes/no\n");
				
				bzero(buffer, string_length);

				fgets(res, string_length, stdin); //Client Command Input
				res[strlen(res)-1] = '\0';

				sprintf(buffer,"receive %s %s %s",res,cmd2,cmd4);

				//Send string to Server
				send(clientSocket, buffer, strlen(buffer), 0);
				}else{
					printf("Server:\t%s\n", buffer);
					bzero(buffer, string_length);
				}
			}
		}

		if (FD_ISSET(0, &readfds) )
		{
			pass = 0;
			total_client = 0;
			total_file = 0;

			// printf("Client:\t");
			
			//Clear the whole String
			bzero(buffer, string_length);
			bzero(cmndStart, string_length);
			bzero(field2, string_length);
			bzero(field3, string_length);
			bzero(field4, string_length);

			fgets(buffer, string_length, stdin); //Client Command Input

			// sscanf(buffer, "%s", cmndStart);	//Keep record Command First Part
			sscanf(buffer, "%s %s %s %s", cmndStart, field2, field3, field4);

			//Send string to Server
			send(clientSocket, buffer, strlen(buffer), 0);

			//For View all Users
			if(strcmp(cmndStart, "/users") == 0 && checkCommand(buffer) == 1)
			{
				// Receive Client Number by String and Convert to Integer
				recv(clientSocket, &total_client, sizeof(total_client), 0);

				printf("Server: ");
				printf("Total Client: %d\n", total_client);
				
				for(x=0; x < total_client; x++)
				{
					bzero(buffer, string_length);
					recv(clientSocket, buffer, string_length, 0);
					printf("\t%s\n", buffer);
					bzero(buffer, strlen(buffer));					
				}				
			}


			//For View all Files with Details
			if(strcmp(cmndStart, "/files") == 0 && checkCommand(buffer) == 2)
			{
				// Receive Client Number by String and Convert to Integer
				recv(clientSocket, &total_file, sizeof(total_file), 0);

				printf("Server: ");
				printf("Total File: %d\n", total_file);
				
				for(x=0; x < total_file; x++)
				{
					bzero(buffer, string_length);
					recv(clientSocket, buffer, string_length, 0);
					printf("\t%s\n", buffer);
					bzero(buffer, strlen(buffer));					
				}				
			}


			//For Upload File
			if(strcmp(cmndStart, "/upload") == 0 && checkCommand(buffer) == 3) 
			{
				bzero(buffer, string_length);
				recv(clientSocket, buffer, string_length, 0);
				
				if(strcmp(buffer, "accept") == 0)
				{
					uploadFile(field2, clientSocket);
				}			
							
			}


			//For Download File
			if(strcmp(cmndStart, "/download") == 0 && checkCommand(buffer) == 4) 
			{
				bzero(buffer, string_length);
				recv(clientSocket, buffer, string_length, 0);
				
				if(strcmp(buffer, "accept") == 0)
				{
					download_ServerFile(field2, clientSocket);
				}					
			}



			//For Read File
			if(strcmp(cmndStart, "/read") == 0 && checkCommand(buffer) == 6) 
			{
				bzero(buffer, string_length);
				recv(clientSocket, buffer, string_length, 0);
				
				if(strcmp(buffer, "Valid") == 0)
				{
					ReadFile(clientSocket);				
				}

			}



			//For Exit Command
			if(strcmp(cmndStart, "/exit") == 0 && checkCommand(buffer) == 9)
			{
				bzero(buffer, string_length);	
				bzero(cmndStart, string_length);
						
				printf("Disconnected from server.\n");
				close(clientSocket);
				exit(1);
				break;
			}



			bzero(buffer, string_length);
			if(recv(clientSocket, buffer, string_length, 0) < 0){
				printf("Error in receiving data.\n");
			}
			else
			{
				printf("Server:\t%s\n", buffer);
				bzero(buffer, string_length);
			}	
		}	
	}

	return 0;
}