#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h> 
#include <errno.h>
#include <sys/stat.h>

#define server_ipaddress "127.0.0.1"
#define LENGTH 512
#define PORT 50000
#define MAX 100


/*function to get size of the file.*/
long int findSize(const char *file_name)
{
    struct stat st; /*declare stat variable*/
     
    /*get the size using stat()*/
     
    if(stat(file_name,&st)==0)
        return (st.st_size);
    else
        return -1;
}


int main()
{

	fd_set file_desc_exit;
	int WAIT = 99999999;
	char option_format_tcon[MAX];									//Char buffer for storing tconnect command from user input
	char option_format_tget[MAX];									//Char buffer for storing tget command from user input
	char option_format_tput[MAX];									//Char buffer for storing tput command from user input
	char file_name_on_server[MAX] = {""};

	// Printing on STDOUT
	printf("3 operations can be done from this client: \n");
	printf("1. tconnect <IP address> <Username> <Password> \n");
	printf("2. tget <Filename> \n");
	printf("3. tput <Filename> \n");

	printf("First connect to the server for using tget and tput functions \n");

	printf("Enter the correct format of your operation \n");
	
	fgets(option_format_tcon, sizeof(option_format_tcon),stdin);						//Read user tconnect command from STDIN
	


	// Logic used for dividing the tconnect user input into individual words
	char splitStrings_tcon[10][10] = {""};
	int j_tcon=0; int cnt_tcon=0; int i_tcon;
    for(i_tcon=0;i_tcon<=(strlen(option_format_tcon));i_tcon++)
    {
        // if space or NULL found, assign NULL into splitStrings[cnt]
        if(option_format_tcon[i_tcon]==' '||option_format_tcon[i_tcon]=='\0')
        {
            splitStrings_tcon[cnt_tcon][j_tcon]='\0';
            cnt_tcon++;  //for next word
            j_tcon=0;    //for next word, init index to 0
        }
        else
        {
            splitStrings_tcon[cnt_tcon][j_tcon]=option_format_tcon[i_tcon];
            j_tcon++;
        }
    }

    

    //tconnect function starts from here

    if (strcmp(splitStrings_tcon[0], "tconnect") == 0)						//Checks if user entered command is "tconnect"
    {
		int socket_id_client_to_server;                                     // Created a socket with name sokcet_id_client
		struct sockaddr_in Tiger_client;

		socket_id_client_to_server = socket(AF_INET, SOCK_STREAM, 0);       // Define properties to the socket

		// These are details of the server the client
		Tiger_client.sin_family = AF_INET;					// IPv4 address family		
		Tiger_client.sin_addr.s_addr = INADDR_ANY;			// since we are connecting the client and the server on the local machine
		Tiger_client.sin_port = htons(PORT);				// port number

		 // Convert IPv4 address from text to binary form and check if it is a valid address
  		if(inet_pton(AF_INET, splitStrings_tcon[1] ,&Tiger_client.sin_addr) <= 0)
  		{
        printf("\nInvalid address entered. Please enter valid IP address\n"); 
		return 1; 
		} 
		if (strcmp(splitStrings_tcon[2],"user") == 0)					// Check if username entered is correct
		{
			if (strcmp(splitStrings_tcon[3], "pass\n") == 0)			// Check if password entered is correct
			{
				int connection = connect(socket_id_client_to_server, (struct sockaddr *) &Tiger_client, sizeof(Tiger_client));		// Connect to the server scoket
				if (connection != 0)
				{
					printf("Some error occured while connecting the server \n");		// Error message is connection fails
					printf("(errno = %d)\n",errno);
					close(socket_id_client_to_server);									// Close the socket
					return 1;
				}
				else
				{
					printf("\nConnection to the server established \n");

					char servermessage[255]= {""};
					recv(socket_id_client_to_server, &servermessage, sizeof(servermessage), 0);		// Receive Welcome messaage from the server
					printf("\nThe received message from the server is %s\n", servermessage);
				}







				// tget() starts from here
				fgets(option_format_tget, sizeof(option_format_tget), stdin);						// Read tget command from STDIN


				// To remove '\n' 
				int len_get = strlen(option_format_tget);
				if( option_format_tget[len_get-1] == '\n' )
				{
				    option_format_tget[len_get-1] = 0;
				}

				
				// Logic used for dividing the tget user input into individual words
				char splitStrings_tget[10][100] = {""};
				int j_tget=0; int cnt_tget=0; int i_tget;
			    for(i_tget=0;i_tget<=(strlen(option_format_tget));i_tget++)
			    {
			        // if space or NULL found, assign NULL into splitStrings[cnt]
			        if(option_format_tget[i_tget]==' '||option_format_tget[i_tget]=='\n')
			        {
			            splitStrings_tget[cnt_tget][j_tget]='\0';
			            cnt_tget++;  //for next word
			            j_tget=0;    //for next word, init index to 0
			        }
			        else
			        {
			            splitStrings_tget[cnt_tget][j_tget]=option_format_tget[i_tget];
			            j_tget++;
			        }
			    }



				if(strcmp(splitStrings_tget[0],"tget") == 0)									// Check if user has entered tget function
				{

					char num_2[100] = "2";
					send(socket_id_client_to_server, &num_2, sizeof(num_2), 0);					// Send server num_2 to tell that client want to download a file

					char file_name_tget[100];													// Character buffer to save file name
			    	memset(file_name_tget, '\0', sizeof(file_name_tget));

			    	char file_buffer_tget[LENGTH];												// Character buffer to store file size
			    	memset(file_buffer_tget, '\0', sizeof(file_buffer_tget));

			    	strcpy(file_name_tget, splitStrings_tget[1]);
			    	send(socket_id_client_to_server, &file_name_tget, sizeof(file_name_tget), 0);		// Sends file name to the server
			    	printf("[Client] Getting %s from the Server...", file_name_tget);

			    	char file_location_tget[255] ="/home/user/Desktop/client_storage/";								// First part of the client file address
					strcat(file_location_tget, file_name_tget);								// Concatination of first and second part of the file address
							
			    	char* final_locationoftget= "";
			    	final_locationoftget= (char*) file_location_tget;
					
					
					


					FILE *file_tget_validity = fopen(final_locationoftget, "a");				// Open file with append mode
					if(file_tget_validity == NULL)												// Check if file has that permission
					{
						printf("File %s Cannot be opened.\n", file_name_tget);
					}
					else
					{
						bzero(file_buffer_tget, LENGTH); 
						int file_tget_readsize = 0;
					    while((file_tget_readsize = recv(socket_id_client_to_server, file_buffer_tget, LENGTH, 0)) > 0)					//Receive data from TigerS
					    {
							int write_size = fwrite(file_buffer_tget, sizeof(char), file_tget_readsize, file_tget_validity);			// Write to the file
					        if(write_size < file_tget_readsize)
							{
					            printf("Writing file to the client failed.\n");
					        }
							bzero(file_buffer_tget, LENGTH);
							if (file_tget_readsize == 0 || file_tget_readsize != 512) 
							{
								break;
							}
						}
						if(file_tget_readsize < 0)
				        {
							if (errno == EAGAIN)															// Reference for EAGAIN - GeeksforGeeks.com
							{
								printf("Timed out. Please try again.\n");
							}
							else
							{
								fprintf(stderr, "tget() failed because of errno = %d\n", errno);
							}
						}
					    printf("File received from the server!\n");										//Ignore if it prints file cannot opened
					    fclose(file_tget_validity);
					}
				}
				else
				{
					printf("Incorrect operation format entered. Please enter correct format tget<filename.txt>   (errno = %d)\n",errno);
					printf("Now please enter for tput <filename.txt> or start a new connection session with server\n");
				}








				// tput() starts from here
				fgets(option_format_tput,sizeof(option_format_tput),stdin);					// Read tput command from STDIN


				// To remove '\n'
				int len_put = strlen(option_format_tput);
				if( option_format_tput[len_put-1] == '\n' )
				{
				    option_format_tput[len_put-1] = 0;
				}

				
				
				// Logic used for dividing the tconnect user input into individual words
				char splitStrings_tput[10][10] = {""};
				int j_tput=0; int cnt_tput=0; int i_tput;
			    for(i_tput=0;i_tput<=(strlen(option_format_tput));i_tput++)
			    {
			        // if space or NULL found, assign NULL into splitStrings[cnt]
			        if(option_format_tput[i_tput]==' '||option_format_tput[i_tput]=='\0')
			        {
			            splitStrings_tput[cnt_tput][j_tput]='\0';
			            cnt_tput++;  //for next word
			            j_tput=0;    //for next word, init index to 0
			        }
			        else
			        {
			            splitStrings_tput[cnt_tput][j_tput]=option_format_tput[i_tput];
			            j_tput++;
			        }
			    }






			    if (strcmp(splitStrings_tput[0], "tput") == 0)							// Check if user entered correct tput function
			    {

			    	char num_3[100] = "3";
					send(socket_id_client_to_server, &num_3, sizeof(num_3), 0);					// Send server num_3 to tell that client want to upload a file
			    	char file_name_tput[100];
			    	memset(file_name_tput, '\0', sizeof(file_name_tput));

			    	char file_buffer_tput[LENGTH];
			    	memset(file_buffer_tput, '\0', sizeof(file_buffer_tput));

			    	strcpy(file_name_tput, splitStrings_tput[1]);
			    	send(socket_id_client_to_server, &file_name_tput, sizeof(file_name_tput), 0);		// Sends file name to the server
			    	printf("[Client] Sending %s to the Server...", file_name_tput);

			    	char file_location_tput[255] ="/home/user/Desktop/client_storage/";								// First part of the client file address
					strcat(file_location_tput, file_name_tput);								// Concatination of first and second part of the file address
							
			    	char* final_location = file_location_tput;
			    	//final_location = (char*) file_location_tput;

					FILE *file_tput_validity = fopen(final_location, "r");					// Open file with read mode
					if(file_tput_validity == NULL)											// Check for file permission
					{
						printf("ERROR: File %s not found.\n", final_location);
						exit(1);
					}


					// Finds the size of file
					// long int size=0;
					// size=findSize(final_location);

					// unsigned NetInt;

					// NetInt=htonl((unsigned)size);
					// send(socket_id_client_to_server,(char*)&NetInt, sizeof(long),0);



					bzero(file_buffer_tput, LENGTH); 
					int file_tput_readsize;
					
					while((file_tput_readsize = fread(file_buffer_tput, sizeof(char), LENGTH, file_tput_validity)) > 0)				// Reads the content of the file
					{	

					    if(send(socket_id_client_to_server, file_buffer_tput, file_tput_readsize, 0) < 0)							// Sends the content to the server
					    {
					        printf("ERROR: sending file %s to the client failed due to (errno = %d)\n", final_location, errno);
					        break;
					    }

					    bzero(file_buffer_tput, LENGTH);
					}
					printf("File %s from Client was sent successfully!\n", file_name_tput);
				}
				else
				{
					printf("Incorrect operation format entered. Please enter correct format tput<filename.txt>  (errno = %d) \n",errno);
					printf("Please start  new connection session for using tput <filename.txt>\n");
				}


				// Exit the socket
				char exit_msg[100];
				fgets(exit_msg,sizeof(exit_msg),stdin);
				if(strcmp(exit_msg,"exit\n") == 0)
				{
					printf("Client is disconnected from the server \n");
					send(socket_id_client_to_server, exit_msg, sizeof(exit_msg), 0);
					close(socket_id_client_to_server);
				}

			}
			else
			{
				printf("Password incorrect. Try again with the correct password \n");
					printf("(errno = %d)\n",errno);
					return 1;
			}
		}
		else
		{
			printf("Incorrect username entered \n");
		}
	}
	else
	{
		printf("First connect to the server. Please enter correct operation format \n");
		return 1;
	}
	return 0;
}



