# TCP_Based_Online_File_Collaboration_Tool

Compilation command for server :  gcc server.c -o server
Running command for server :   ./server

Compilation command for client :  gcc client.c -o client 
Running command for client :   ./client 


Commands to be given as:
1. /users					: View all active clients.
   
2. /files					: View all files that have been uploaded to the server, along with all details (owners, collaborators, permissions), and the no. of lines in 						  the file.

3. /upload <filename>				: Upload the local file filename to the server.
						  Example: /upload a.txt 

4. /download <filename>				: Download the server file filename to the client, if given client has permission to access that file.
						  Example: /download a.txt


5. /invite <filename> <client_id> <permission>	: Invite client client_id to join as collaborator for file filename (should already be present on server). permission can be either of V/E 						  	  signifying viewer/editor.
						  Example: /invite a.txt 10001 V  [Permission for View]
							   /invite a.txt 10001 E  [Permission for Edit]


6. /read <filename> <start_idx> <end_idx>	: Read from file filename starting from line index start_idx to end_idx . If only one index is specified, read that line. If none are 						  		  specified, read the entire file.
						  Example: /read a.txt 0 5   [Read Multiple Lines]
							   /read a.txt 5     [Read Single Lines]
							   /read a.txt 	     [Read All Lines from File]


7. /insert <filename> <idx> “<message>”		: Write message at the line number specified by idx into file filename . If idx is not specified, insert at the end. Quotes around the 								  message to demarcate it from the other fields.
						  Example: /insert a.txt 5 “New Message Inserted.”
							   /insert a.txt “New Message Inserted.”	
	

8. /delete <filename> <start_idx> <end_idx>	: Delete lines starting from line index start_idx to end_idx from file filename. If only one index is specified, delete that line. If none 							  are specified, delete the entire contents of the file.
						  Example: /delete a.txt 0 5    [Delete Multiple Lines]
							   /delete a.txt 5	[Delete Single Lines]
							   /delete a.txt 	[Delete all Line from file]

9. /exit					: Disconnects from the server, and then all files which this client owned should be deleted at the server, and update the permission file.
						 Example: /exit
