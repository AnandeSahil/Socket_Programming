#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>        
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


#define PORT 50000
#define LENGTH 512


// Gets the time now.
struct tm * getTime(){
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	return timeinfo;
}



int current_client_count = 0; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;                  // Starts the mutex. Assigned the task to a single thread
																	// and waits until its completion and then assignes it to
																	// other threads

typedef struct 														// Structure defined for socket parameters
{
	int socket_id;
	struct sockaddr_in address;
	char buffer[100];
}client_info;


void *server_func (void * arg)										// Server funtions
{
	int WAIT = 99999999;
	char buffer[100] = {""};
	struct tm * timeinfo = getTime();
	
	char servermessage[255] = "Welcome to the Tiger_server";		
	client_info * client_xyz = (client_info *) arg;
	send(client_xyz -> socket_id, servermessage, sizeof(servermessage), 0);		// Sends message to the client after it connects to the server
	printf("(%02d:%02d:%02d) Socket ID is %d\n",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, client_xyz -> socket_id );						// Prints the socket ID on the STDOUT
	

	char option_num_2[100] = {""};														// Char buffer for checking if client want to perform option 3 operation
	recv(client_xyz -> socket_id, &option_num_2, sizeof(option_num_2), 0);				// Receieve the tput option number from client
																						
	

	// tget() starts here
	if ((strcmp(option_num_2,"2") == 0))
	{

		pthread_mutex_lock(&mutex);															// Locks mutex to the client thread
		char file_size_tget[LENGTH];
		memset(file_size_tget, '\0', sizeof(file_size_tget));
		
		char file_name_to_download[255];
		memset(file_name_to_download, '\0', sizeof(file_name_to_download));
		recv(client_xyz -> socket_id, file_name_to_download, sizeof(file_name_to_download), 0);					// Receive filename client wants to download

		printf("file to download from server is %s\n", file_name_to_download);

		char file_location_tget[255] = "/home/user/Desktop/server_storage/";
		strcat(file_location_tget, file_name_to_download);								// Concatination of first and second part of the file address

		char* final_location_tget = file_location_tget;
	
		
		FILE *file_per_tget = fopen(final_location_tget, "r");							// Opens file with read mode
		if(file_per_tget == NULL)
		{
		    printf("ERROR: File %s not found on server. (errno = %d)\n", file_name_to_download, errno);
		    printf("The connection to the client is disconnected, please connect again\n");
		    exit (1);
		}
		printf("[Server] Sending %s to the Client Socket ID %d...", file_name_to_download, client_xyz -> socket_id);
		bzero(file_size_tget, LENGTH); 
		int file_char_read_tget; 
		while((file_char_read_tget = fread(file_size_tget, sizeof(char), LENGTH, file_per_tget))>0)
		{
		    if(send(client_xyz -> socket_id, file_size_tget, file_char_read_tget, 0) < 0)							// Sends data to the client
		    {
		        printf("ERROR: Failed to send file %s. (errno = %d)\n", file_name_to_download, errno);
		        exit(1);
		    }
		    bzero(file_size_tget, LENGTH);
		}
		printf("File sent to client socket id %d successfully!\n", client_xyz -> socket_id);
		pthread_mutex_unlock(&mutex);
	}









	// tput() starts here
	char option_num_3[100] = {""};														// Char buffer for checking if client want to perform option 3 operation
	recv(client_xyz -> socket_id, &option_num_3, sizeof(option_num_3), 0);			// Receieve the tput  option number from client

	if ((strcmp(option_num_3,"3") == 0))
	{

		pthread_mutex_lock(&mutex);
		char file_buffer_tput[LENGTH];
		memset(file_buffer_tput, '\0', sizeof(file_buffer_tput));
		
		char file_name_to_upload[100];
		memset(file_name_to_upload, '\0', sizeof(file_name_to_upload));
		recv(client_xyz -> socket_id, file_name_to_upload, sizeof(file_name_to_upload), 0);				// Receive filename that client wants to upload

		printf("file to upload on server is %s\n", file_name_to_upload);

		char file_location_tput[255] = "/home/user/Desktop/server_storage/";
		strcat(file_location_tput, file_name_to_upload);								// Concatination of first and second part of the file address

									
    	char* final_locationoftput= "";
    	final_locationoftput= (char*) file_location_tput;
		
		
		FILE *file_tput_validity = fopen(final_locationoftput, "a");
		if(file_tput_validity == NULL)
		{
			printf("File %s Cannot be opened.\n", file_name_to_upload);
		}
		else
		{
			bzero(file_buffer_tput, LENGTH); 
			int file_tput_readsize = 0;
		    while((file_tput_readsize = recv(client_xyz -> socket_id, file_buffer_tput, LENGTH, 0)) > 0)
		    {
				int write_size = fwrite(file_buffer_tput, sizeof(char), file_tput_readsize, file_tput_validity);			// Write the data recieved from the client 
		        if(write_size < file_tput_readsize)
				{
		            printf("File write failed.\n");
		        }
				bzero(file_buffer_tput, LENGTH);
				if (file_tput_readsize == 0 || file_tput_readsize != 512) 
				{
					break;
				}
			}
			if(file_tput_readsize < 0)
	        {
				if (errno == EAGAIN)
				{
					printf("recv() timed out.\n");
				}
				else
				{
					fprintf(stderr, "recv() failed due to errno = %d\n", errno);
				}
			}
		    printf("File received at server!\n");
		    fclose(file_tput_validity);
		    pthread_mutex_unlock(&mutex);
		}
	}

	char exit_confirm[255] = "exit\n";
	char exit_msg[255] = "";
	recv(client_xyz -> socket_id, &exit_msg, sizeof(exit_msg), 0);
	if (strcmp(exit_msg,exit_confirm) == 0)
	{
	printf("(%02d:%02d:%02d) Socket ID %d disconnected\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, client_xyz -> socket_id);
	pthread_exit(NULL);
	}

}




int main() 
{
	int socket_server_id;                                  // Created a socket with name socket_id_server
	struct sockaddr_in Tiger_server;
	struct sockaddr_in Tiger_client;

	int serverAddrlen = sizeof(Tiger_server);
	int clientAddrlen = sizeof(Tiger_client);

	memset(&Tiger_server, 0, serverAddrlen);
	memset(&Tiger_client, 0, clientAddrlen);

	socket_server_id = socket(AF_INET, SOCK_STREAM, 0);       // Define properties to the socket
	
	Tiger_server.sin_family = AF_INET;					// IPv4 address family
	Tiger_server.sin_addr.s_addr = INADDR_ANY;			// Any ip address on the local machine 
	Tiger_server.sin_port = htons(PORT);				//Port Number

	int status_binding = bind(socket_server_id, (struct sockaddr *) &Tiger_server, serverAddrlen);		// Bind the socket with the socket parameters mentioned above
	if (status_binding != 0)
	{
		printf("The socket is not binded succesfully \n");
		return 1;
	}
	else
	{
		printf("The socket binded succesfully \n");
	}

	int status_listen = listen(socket_server_id,1);			// Server socket listens for any clients sockets 
	if (status_listen != 0)
	{
		printf("Error occured while server was trying to listen for client request \n");
		return 1;
	}
	else
	{
		printf("Server has started to listen for clients request succesfully \n");
	}

	while (1)
	{
		int server_socket_connected_to_client_id;
		server_socket_connected_to_client_id = accept(socket_server_id, NULL, NULL);		// Server Socket accepts the client socket connection


		if (server_socket_connected_to_client_id < 0)										// If accept function is successful it will give
		{																					// value greater than 0
			printf("No Client to connect to the Server or Error occured while connecting a client \n");
			return -1;
		}
		else
		{
			client_info *client_xyz = (client_info*) malloc(sizeof(client_info));
			client_xyz -> address = Tiger_client;
			client_xyz -> socket_id = server_socket_connected_to_client_id;

			current_client_count++;
			pthread_t id;																	// Create a thread
			if ( pthread_create(&id, NULL, &server_func, (void*) client_xyz) != 0)			// Pass function which the thread needs to perform as an argument
			{
				printf("Failed to create a thread \n");
			}
		}
	}
	return 0;
}