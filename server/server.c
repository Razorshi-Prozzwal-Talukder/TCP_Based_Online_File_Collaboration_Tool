#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <regex.h> 
	
#define TRUE 1
#define FALSE 0
#define PORT 5050

const int MAX_CLIENT = 5;
const int string_length = 1024;
int id = 10000;
int fileStore_index = 0; 			// This index is used for keep track in File_DataStructure


// Structure For Client Store Data
struct client_DataStore{
	int unique_ID;
	int client_socket_id;
	int client_port;
	int client_status;
};


// Structure For File Related Information
struct file_DataStore{
	char file_Name[100];
	int client_unique_ID;
	int total_line;
};
//Keep the File Array Structure Name Globally
struct file_DataStore file_Information[1000];


//To Check is there any File in Server Site or Not
int fileInStore(char filename[],int client_unq_ID){
	for(int x =0; x<fileStore_index; x++){
		if(strcmp(file_Information[x].file_Name,filename)==0 && file_Information[x].client_unique_ID == client_unq_ID){
			return 1;
		}
	}
	return 0;
}


//Count total Line in File
int File_total_Line(char filename[]) 
{
   FILE *fp;
   char ch;
   int linesCount=0;
   //open file in read more
   fp=fopen(filename,"r");
   if(fp==NULL) {
      printf("File \"%s\" does not exist!!!\n",filename);
      return -1;
   }
   //read character by character and check for new line
   while((ch=fgetc(fp))!=EOF) {
      if(ch=='\n')
         linesCount++;
   }
   //close the file
   fclose(fp);
   return linesCount;
}


//Insert Line in File from Server
void InsertFile(int clientSocket,char filename[], int start_idx, char field5[])
{
	int len,lines=0, total_lines;
	FILE *fp, *fp2;
	char buffer[string_length], c, message[string_length];

	// if(strlen(field3)>0 && strlen(field4)>0) strcpy(message, field4);
	// else 
	strcpy(message, field5);
	fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}

	fp2=fopen("newFile.txt","w");
	if(!fp2)
	{
		printf("Error in opening.\n");
		exit(1);
	}

	rewind(fp);

	for(int i =0; i<start_idx; i++){
		fscanf(fp,"%[^\n]\n",buffer);
		fprintf(fp2,"%s\n",buffer);
		bzero(buffer,strlen(buffer));
	}

	//insertion works on here
	fprintf(fp2,"%s\n",message);		
	bzero(buffer,strlen(buffer));

	total_lines = File_total_Line(filename);

	for(int j =start_idx; j<total_lines; j++)
	{
		fscanf(fp,"%[^\n]\n",buffer);
		fprintf(fp2,"%s\n",buffer);		
		bzero(buffer,strlen(buffer));
	}
	
	bzero(buffer,strlen(buffer));
	fflush(stdin);
	fclose(fp);
	fclose(fp2);

	fp2=fopen("newFile.txt","r");
	if(!fp2)
	{
		printf("Error in opening.\n");
		exit(1);
	}


	fp=fopen(filename,"w");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}
  
    // Read contents from file
    c = fgetc(fp2);
    while (c != EOF)
    {
        fputc(c, fp);
        c = fgetc(fp2);
    }
  
    fclose(fp2);
    fclose(fp);

	total_lines = File_total_Line(filename);

	for(int j=0; j<fileStore_index; j++)
	{
		if(strcmp(file_Information[j].file_Name, filename) == 0)
		file_Information[j].total_line = total_lines;
	}
}



//Delete File from Server
void DeleteFile(int clientSocket,char filename[], int first_indx,int last_index)
{
	int len,lines=0, total_lines;
	FILE *fp, *fp2;
	char buffer[string_length], c;

	fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}

	fp2=fopen("newFile.txt","w");
	if(!fp2)
	{
		printf("Error in opening.\n");
		exit(1);
	}

	lines = last_index - first_indx + 1;

	rewind(fp);

	for(int i =0; i<first_indx; i++){
		fscanf(fp,"%[^\n]\n",buffer);
		fprintf(fp2,"%s\n",buffer);
		bzero(buffer,strlen(buffer));
	}

	for(int j =0; j<lines; j++){
		fscanf(fp,"%[^\n]\n",buffer);		
		bzero(buffer,strlen(buffer));
	}

	total_lines = File_total_Line(filename);

	for(int j =lines+first_indx; j<total_lines; j++)
	{
		fscanf(fp,"%[^\n]\n",buffer);
		fprintf(fp2,"%s\n",buffer);		
		bzero(buffer,strlen(buffer));
	}
	
	bzero(buffer,strlen(buffer));
	fflush(stdin);
	fclose(fp);
	fclose(fp2);

	fp2=fopen("newFile.txt","r");
	if(!fp2)
	{
		printf("Error in opening.\n");
		exit(1);
	}


	fp=fopen(filename,"w");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}
  
    // Read contents from file
    c = fgetc(fp2);
    while (c != EOF)
    {
        fputc(c, fp);
        c = fgetc(fp2);
    }
  
    fclose(fp2);
    fclose(fp);

	total_lines = File_total_Line(filename);

	for(int j=0; j<fileStore_index; j++)
	{
		if(strcmp(file_Information[j].file_Name, filename) == 0)
		file_Information[j].total_line = total_lines;
	}
}



//Read File from Server
void ReadFile(int clientSocket,char filename[], int first_indx,int last_index){
	int len,lines=0;
	FILE *fp;
	char buffer[string_length];

	fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error in opening.\n");
		exit(1);
	}
	lines = last_index - first_indx + 1;
	send(clientSocket,&lines,sizeof(lines),0);
	bzero(buffer,strlen(buffer));

	rewind(fp);

	for(int i =0; i<first_indx; i++){
		fscanf(fp,"%[^\n]\n",buffer);
		bzero(buffer,strlen(buffer));
	}

	for(int j =0; j<lines; j++){
		fscanf(fp,"%[^\n]\n",buffer);
		send(clientSocket,buffer,string_length,0);
		bzero(buffer,strlen(buffer));
	}
	// printf("\nFile Uploaded Successfully!!\n");

	recv(clientSocket,&lines,sizeof(lines),0);
	bzero(buffer,strlen(buffer));
	fflush(stdin);
	fclose(fp);
}


//Convert String to Integer 
int stringToInteger(char *str)
{
    char c;
    int result = 0;
    int sign = 1;   // default to no minus. 
    // Check for minus
    if (str[0] == '-') 
    {
      str++;   // Skip over the minus.
      sign = -1;  // It's a negative number. 
    }

	// Stop the loop if we are at the end of the string.
    while((c = *str))   
    {
       // See if it's valid digit
       if (c >= '0' && c <= '9') 
       {
          result *= 10;
          result += c - '0';
       }
       else // Not a valid digit
       {
           break; // Stop looking for more digits. 
       }
       str++;  // Go to next digit. 
    }
    return result * sign;
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


//Server File Upload Portion
void Client_DownloadFile(char filename[],int clientSocket)
{
	int len,b=0;
	FILE *fp;
	char buffer[string_length];

	fp=fopen(filename,"r");
	if(!fp)
	{
		printf("Error in opening. File not Existing.\n");
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


//Download_Upload File in Server Site
void Client_UploadFile(char filename[],int clientSocket)
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
	else if(strcmp("/insert",firstPart) == 0)
	{
		/* Execute regular expression */
		reti = regexec(&regex, secondPart, 0, 0, 0);
		if (!reti) {
			return 7;
		}
		regfree(&regex);		
	}
	else if(strcmp("/delete",firstPart) == 0)
	{
		/* Execute regular expression */
		reti = regexec(&regex, secondPart, 0, 0, 0);
		if (!reti) {
			return 8;
		}
		regfree(&regex);		
	}  
	else if (strcmp("/exit",firstPart)==0 && strlen(secondPart)==0 && strlen(thirdPart)==0 && strlen(fourthPart)==0) return 9;
	else return 0;
}



//To remove Quotation from Message
void removeQuotation(char str[])
{
	int i, j, len;
	
	len = strlen(str);
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == '"')
		{
			for(j = i; j < len; j++)
			{
				str[j] = str[j + 1];
			}
			len--;
			i--;	
		} 
	}
}


//Check File Owner For Given Permission
int FileOwner(char field2[], int clientsocket)
{
	int x, y, z;
	for(x=0 ; x<fileStore_index; x++)
	{
		if(strcmp(file_Information[x].file_Name, field2) == 0 && file_Information[x].client_unique_ID == clientsocket)
		{
			return 1;
		}	
	}
}

	
int main(int argc , char *argv[])
{
	int clientCount = 0, start_idx, end_idx, ttl_f_line;
	char field1[string_length], field2[string_length], field3[string_length], field4[string_length], field5[string_length], str[string_length];
	int client_list = 0, x, y, unique_fileName;
	int opt = TRUE;
	int master_socket , addrlen , new_socket,
		max_clients = MAX_CLIENT , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;

	struct client_DataStore *dataStore, temp, client_socket[MAX_CLIENT];
		
	char buffer[string_length], cmndStart[string_length]; 

	
	fd_set readfds;
			
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i].client_socket_id = 0;
		client_socket[i].client_port = 0;
		client_socket[i].client_status = 0;
	}
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}
	
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//try to specify maximum of 5 pending connections for the master socket
	if (listen(master_socket, 5) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
		
	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i].client_socket_id;
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds) )
		{
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			if(client_list < max_clients)
			{
				//inform user of socket number - used in send and receive commands
				printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					
				//add new socket to array of sockets
				for (i = 0; i < max_clients; i++)
				{
					//if position is empty
					if( client_socket[i].client_socket_id == 0 )
					{
						client_list++;

						client_socket[i].client_socket_id = new_socket;
						client_socket[i].unique_ID = id++;
						client_socket[i].client_port = ntohs(address.sin_port);
						client_socket[i].client_status = 1;

						printf("Adding to list of sockets as %d\n" , i);
						strcpy(buffer, "Connected");
						send(new_socket, buffer, strlen(buffer), 0);
						bzero(buffer, sizeof(buffer));
						break;
					}
				}
			}
			else
			{
				strcpy(buffer, "limit_cross");
				send(new_socket, buffer, strlen(buffer), 0);
				bzero(buffer, sizeof(buffer));
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i].client_socket_id;
			int client_unq_ID = client_socket[i].unique_ID;
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the incoming message
				
				//Default Client Off kora --- ctrl + c
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n",inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i].client_socket_id = 0;
					client_socket[i].client_port = 0;
					client_socket[i].client_status = 0;
					client_list--;
					break;
				}
				
				buffer[strlen(buffer)-1] = '\0';

				printf("Client: %s\n", buffer);


				//Clear the Whole String
				bzero(cmndStart, string_length);
				bzero(field2, string_length);
				bzero(field3, string_length);
				bzero(field4, string_length);



				//Keep record Command First Part
				sscanf(buffer, "%s %s %s %s", cmndStart, field2, field3, field4); 

				//View All Active Clients
				if(strcmp(cmndStart, "/users") == 0 && checkCommand(buffer) == 1)
				{
					bzero(cmndStart, string_length);
					bzero(buffer, string_length);

					send(sd, &client_list, sizeof(client_list), 0);
					bzero(buffer, strlen(buffer));

					for(x = 0; x < max_clients; x++)
					{
						if(client_socket[x].client_status == 1)
						{
							bzero(buffer, strlen(buffer));

							//client details pass
							sprintf(buffer, "Client Unique ID: %d\tClient Socket ID: %d\tClient Port Number: %d\n", client_socket[x].unique_ID, client_socket[x].client_socket_id, client_socket[x].client_port);

							send(sd, buffer, string_length, 0);
							bzero(buffer, strlen(buffer));
						}
					}
					bzero(buffer, strlen(buffer));
					strcpy(buffer, "Displayed All Clients.\n");					
				}


				//View All Files Details
				else if(strcmp(cmndStart, "/files") == 0 && checkCommand(buffer) == 2)
				{
					bzero(cmndStart, string_length);
					bzero(buffer, string_length);

					send(sd, &fileStore_index, sizeof(fileStore_index), 0);
					bzero(buffer, strlen(buffer));
					

					for(x = 0; x < fileStore_index; x++)
					{
						bzero(buffer, strlen(buffer));

						//File details pass
						sprintf(buffer, "File Name: %s\tTotal Line: %d\tOwner ID: %d\n", file_Information[x].file_Name, file_Information[x].total_line,file_Information[x].client_unique_ID);

						send(sd, buffer, string_length, 0);
						bzero(buffer, strlen(buffer));
					
					}
					bzero(buffer, strlen(buffer));
					if(fileStore_index == 0) strcpy(buffer, "No Files Stored.\n");
					else strcpy(buffer, "Displayed All Files.\n");					
				}


				//Downlod Client Upload File
				else if(strcmp(cmndStart, "/upload") == 0 && checkCommand(buffer) == 3)
				{
					// Check File Name Unique or Not
					unique_fileName = 1;

					/*If there same file is present in Server, then 
					Denied the operation for Uploading.*/

					for(y=0; y < fileStore_index; y++)
					{
						if(strcmp(file_Information[y].file_Name,field2)==0)
						{
							bzero(buffer, strlen(buffer));
							
							strcpy(buffer, "reject");
							send(sd, buffer, strlen(buffer), 0);
							usleep(9000);	

							bzero(buffer, string_length);
							strcpy(buffer, "Operation Denied. Same File Name Exist.\n");

							unique_fileName = 0;
							break;	
						}
					}

					//If Unique, then Upload file and Save all Datas
					if(unique_fileName)
					{
						bzero(buffer, strlen(buffer));							
						strcpy(buffer, "accept");
						send(sd, buffer, strlen(buffer), 0);

						Client_UploadFile(field2, sd);
						y=0;
						while(y < max_clients)
						{
							if(client_socket[y].client_socket_id == sd && client_socket[y].client_status == 1)
							{
								strcpy(file_Information[fileStore_index].file_Name, field2);
								file_Information[fileStore_index].client_unique_ID = client_socket[y].unique_ID;
								file_Information[fileStore_index].total_line = File_total_Line(field2);
								fileStore_index++;
								break;
							}
							y++;
						}
						
						bzero(buffer, strlen(buffer));
						strcpy(buffer, "Uploaded Successfully.\n");
					}	

					// // print file structure working properly or not
					// for(y=0; y<fileStore_index; y++)
					// {
					// 	printf("\nID: %d\tFile name: %s\tTotal Line:%d\n", file_Information[y].client_unique_ID, file_Information[y].file_Name, file_Information[y].total_line);
					// }			
				}


				//Upload Server File
				else if(strcmp(cmndStart, "/download") == 0 && checkCommand(buffer) == 4)
				{
					unique_fileName = 1;					

					for(y=0; y < fileStore_index; y++)
					{
						//Check is this file is Unique file or Not
						if(strcmp(file_Information[y].file_Name,field2)==0 && file_Information[y].client_unique_ID == client_unq_ID)
						{							
							unique_fileName = 0; 
							break;	
						}
					}
					
					/*If the file is like that, its not present in Server site, then
					it will Denied operation. File should be present on Server site and should have permission to access*/
					if(unique_fileName)
					{					
						bzero(buffer, string_length);							
						strcpy(buffer, "reject");
						send(sd, buffer, strlen(buffer), 0);
						usleep(9000);						

						bzero(buffer, string_length);
						strcpy(buffer, "Operation Denied. File Name not Exist or No permission to Download.\n");						
					}
					else 
					{
						// Client_DownloadFile(field2, sd);

						bzero(buffer, string_length);							
						strcpy(buffer, "accept");
						send(sd, buffer, strlen(buffer), 0);

						Client_DownloadFile(field2, sd);

						bzero(buffer, strlen(buffer));
						strcpy(buffer, "Download Successfully.\n");
					}			
				}


				//Invitation Part
				else if(strcmp(cmndStart, "/invite") == 0 && checkCommand(buffer) == 5)
				{
					int client_id = stringToInteger(field3);
					int ownerSocket = 0;

					printf("FileName: %s,\tOwnerSocket: %d,\tInvited Client Unique ID:%d,\tpermission: %s\n", field2, sd, client_id, field4);


					//Get invited Client's Socket ID
					for(x=0 ; x<MAX_CLIENT; x++)
					{
						if(client_socket[x].unique_ID == client_id && client_socket[x].client_status == 1){
							ownerSocket = client_socket[x].client_socket_id;
							break;
						}
					}


					if(FileOwner(field2, sd))
					{
						printf("Filename: %s,\tClientID: %d,\tPermission: %s\n",field2,client_id,field4);
						// strcpy(buffer, "Invite asche.\n");
						sprintf(buffer, "invitation %d %s %s",sd,field2,field4);
						send(ownerSocket, buffer, strlen(buffer), 0);
						bzero(buffer, string_length);
						strcpy(buffer, "Send the Invitation.\n");
						
					}
					else
					{
						printf("File Owner is Not Correct.\n");
						bzero(buffer, string_length);
						strcpy(buffer, "Error! Command Format is not Correct.\nFollow the Instructions carefully.\n");
					}
				}

				//Invitation to Client
				else if(strcmp(cmndStart, "receive") == 0 )
				{
					int client_id=0,ownerSocket=0;

						for(int x =0; x<fileStore_index; x++){
							if(strcmp(file_Information[x].file_Name,field3)==0 ){
								client_id = file_Information[x].client_unique_ID;
								break;
							}
						}

						for(x=0 ; x<MAX_CLIENT; x++)
						{
							if(client_socket[x].unique_ID == client_id ){
								ownerSocket = client_socket[x].client_socket_id;
								break;
							}
						}

					// printf("response: %s, file: %s, permission: %s\n",field2,field3,field4);
					if(strcmp(field2, "yes") == 0){
						printf("Client has accepted the invitation \n");

						sprintf(buffer, "Client has accepted the invitation \n");
						send(ownerSocket, buffer, strlen(buffer), 0);

						bzero(buffer, string_length);
						strcpy(buffer, "Invitation got accepted\n");
					}else{
						printf("Client has not accepted the invitation \n");

						sprintf(buffer, "Client has not accepted the invitation \n");
						send(ownerSocket, buffer, strlen(buffer), 0);

						bzero(buffer, string_length);
						strcpy(buffer, "Invitation not accepted\n");
					}
				}
				

				//For Read Files
				else if(strcmp(cmndStart, "/read") == 0 && checkCommand(buffer) == 6)
				{
					//Read command perfect and file name is also correct.
					//Now, check the number perfect or not
					if(fileInStore(field2,client_unq_ID)==1 && ((strlen(field3)==0 && strlen(field4)==0) || (strlen(field3)>0 && isString_Number(field3) && strlen(field4)==0) || (strlen(field3)>0 && strlen(field4)>0 && isString_Number(field3) && isString_Number(field4))) )
					{
						ttl_f_line = File_total_Line(field2);	

						if(strlen(field3)==0 && strlen(field4)==0)
						{
							start_idx = 0;
							end_idx = ttl_f_line-1;
						}
						else if(strlen(field3)>0 && isString_Number(field3) && strlen(field4)==0)
						{
							start_idx = stringToInteger(field3);
							end_idx = stringToInteger(field3);
						}
						else
						{
							start_idx = stringToInteger(field3);
							end_idx = stringToInteger(field4);
						}

						// printf("file line s %d,e %d,t %d",start_idx,end_idx,ttl_f_line);														

						if(start_idx <= end_idx && start_idx >= -ttl_f_line && start_idx < ttl_f_line && end_idx >= -ttl_f_line && end_idx < ttl_f_line)
						{
							if(start_idx < 0) start_idx = ttl_f_line + start_idx;
							if(end_idx < 0) end_idx = ttl_f_line + end_idx;

							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Valid");
							send(sd, buffer, string_length, 0);
							bzero(buffer, strlen(buffer));

							ReadFile(sd, field2, start_idx, end_idx);

							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Read Successfull");

						}
						else
						{
							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Invalid");
							send(sd, buffer, string_length, 0);

							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Out of Range");
						}									
					}
					else
					{
						bzero(buffer, strlen(buffer));
						strcpy(buffer, "Invalid");
						send(sd, buffer, string_length, 0);

						bzero(buffer, strlen(buffer));
						strcpy(buffer, "File Can't Access.");
					}								
				}


				//For Insert Line in File
				else if(strcmp(cmndStart, "/insert") == 0 && checkCommand(buffer) == 7)
				{
					bzero(field1, string_length);
					bzero(field2, string_length);
					bzero(field3, string_length);
					bzero(field4, string_length);
					bzero(field5, string_length);
					// sscanf(str, "%s %[^\n]", tmpCmnd, tmpStr);

					sscanf(buffer, "%s %s %[^\n]", field1, field2, field5);
					sscanf(field5, "%s %[^\n]", field3, field4);


					//Read command perfect and file name is also correct.

					if(fileInStore(field2,client_unq_ID)==1 && ((isString_Number(field3) && field4[0]=='"' && field4[strlen(field4)-1]=='"') || (field3[0]=='"' && field3[strlen(field3)-1]=='"') || (field3[0]=='"' && field4[strlen(field4)-1]=='"')))
					{
						ttl_f_line = File_total_Line(field2);
						
						if(isString_Number(field3))
						{
							removeQuotation(field4);
							start_idx = stringToInteger(field3);

							bzero(field5, string_length);
							strcpy(field5, field4);
						}
						else
						{
							removeQuotation(field3);
							removeQuotation(field4);
							start_idx = ttl_f_line;

							bzero(field5, string_length);
							strcpy(field5, field3);
							strcat(field5, " ");
							strcat(field5, field4);
						}

						if(start_idx < 0) start_idx = ttl_f_line + start_idx;
						
						if(start_idx >= 0 && start_idx < ttl_f_line )
						{																			
							// InsertFile(sd, field2, start_idx, field3, field4);
							InsertFile(sd, field2, start_idx, field5);

							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Successfully Inserted.");
						}
						else
						{
							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Out of Range");
						}									
					}
					else
					{
						bzero(buffer, strlen(buffer));
						strcpy(buffer, "Can't Access File or Insert Formation is not Correct.");
					}
				}


				else if(strcmp(cmndStart, "/delete") == 0 && checkCommand(buffer) == 8)
				{
					//Check everything For Delete Contant
					if(fileInStore(field2,client_unq_ID)==1 && ((strlen(field3)==0 && strlen(field4)==0) || (strlen(field3)>0 && isString_Number(field3) && strlen(field4)==0) || (strlen(field3)>0 && strlen(field4)>0 && isString_Number(field3) && isString_Number(field4))) )
					{
						ttl_f_line = File_total_Line(field2);	

						if(strlen(field3)==0 && strlen(field4)==0)
						{
							start_idx = 0;
							end_idx = ttl_f_line-1;
						}
						else if(strlen(field3)>0 && isString_Number(field3) && strlen(field4)==0)
						{
							start_idx = stringToInteger(field3);
							end_idx = stringToInteger(field3);
						}
						else
						{
							start_idx = stringToInteger(field3);
							end_idx = stringToInteger(field4);
						}														

						if(start_idx <= end_idx && start_idx >= -ttl_f_line && start_idx < ttl_f_line && end_idx >= -ttl_f_line && end_idx < ttl_f_line)
						{
							if(start_idx < 0) start_idx = ttl_f_line + start_idx;
							if(end_idx < 0) end_idx = ttl_f_line + end_idx;

							DeleteFile(sd, field2, start_idx, end_idx);

							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Delete Successfully");
						}
						else
						{
							bzero(buffer, strlen(buffer));
							strcpy(buffer, "Invalid Index. Out of Range");
						}								
					}
					else
					{
						bzero(buffer, strlen(buffer));
						strcpy(buffer, "No permission to Access / File not Exist.");
					}
				}


				//For Exit Function
				else if(strcmp(cmndStart, "/exit") == 0 && checkCommand(buffer) == 9)
				{
					bzero(buffer, string_length);
					bzero(cmndStart, string_length);

					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n",inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i].client_socket_id = 0;
					client_socket[i].client_port = 0;
					client_socket[i].client_status = 0;
					client_list--;
					break;
				}		

				//Default Section
				else
				{
					bzero(buffer, sizeof(buffer));
					strcpy(buffer, "Error! Command Format is not Correct.\nFollow the Instructions carefully.\n");
				}
				send(sd, buffer, strlen(buffer), 0);
				bzero(buffer, string_length);
			}
		}
	}
		
	return 0;
}
