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


#define PORT 50000
#define LENGTH 512


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

// // Gets the time now.
// struct tm * getTime(){
// 	time_t rawtime;
// 	struct tm * timeinfo;
// 	time( &rawtime );
// 	timeinfo = localtime( &rawtime );
// 	return timeinfo;
// }



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
	
	char servermessage[255] = "Welcome to the Tiger_server";		
	client_info * client_xyz = (client_info *) arg;
	send(client_xyz -> socket_id, servermessage, sizeof(servermessage), 0);		// Sends message to the client after it connects to the server
	printf("Socket ID is %d\n", client_xyz -> socket_id );						// Prints the socket ID on the STDOUT
	

	char option_num_2[100] = {""};														// Char buffer for checking if client want to perform option 3 operation
	recv(client_xyz -> socket_id, &option_num_2, sizeof(option_num_2), 0);			// Receieve the tput 
																					// option number from client
	
	if ((strcmp(option_num_2,"2") == 0))
	{

		//pthread_mutex_lock(&mutex);
		char file_size_tget[LENGTH];
		memset(file_size_tget, '\0', sizeof(file_size_tget));
		
		char file_name_to_download[255];
		memset(file_name_to_download, '\0', sizeof(file_name_to_download));
		recv(client_xyz -> socket_id, file_name_to_download, sizeof(file_name_to_download), 0);

		printf("file to download from server is %s\n", file_name_to_download);

		char file_location_tget[255] = "/home/user/Desktop/server_storage/";
		strcat(file_location_tget, file_name_to_download);								// Concatination of first and second part of the file address

		char* final_location_tget = file_location_tget;
	
		printf("[Server] Sending %s to the Client Socket ID %d...", file_name_to_download, client_xyz -> socket_id);
		FILE *file_per_tget = fopen(final_location_tget, "r");
		if(file_per_tget == NULL)
		{
		    fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", file_name_to_download, errno);
			exit(1);
		}

		bzero(file_size_tget, LENGTH); 
		int file_char_read_tget; 
		while((file_char_read_tget = fread(file_size_tget, sizeof(char), LENGTH, file_per_tget))>0)
		{
		    if(send(client_xyz -> socket_id, file_size_tget, file_char_read_tget, 0) < 0)
		    {
		        fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", file_name_to_download, errno);
		        exit(1);
		    }
		    bzero(file_size_tget, LENGTH);
		}
		printf("File sent to client socket id %d successfully!\n", client_xyz -> socket_id);
		//success = 1;
		//pthread_mutex_unlock(&mutex);
	}








	char option_num_3[100] = {""};														// Char buffer for checking if client want to perform option 3 operation
	recv(client_xyz -> socket_id, &option_num_3, sizeof(option_num_3), 0);			// Receieve the tput 
																					// option number from client
	
	if ((strcmp(option_num_3,"3") == 0))
	{
		//pthread_mutex_lock(&mutex);
		char file_size_tput[LENGTH];
		memset(file_size_tput, '\0', sizeof(file_size_tput));
		
		char file_name_to_upload[255];
		memset(file_name_to_upload, '\0', sizeof(file_name_to_upload));
		recv(client_xyz -> socket_id, file_name_to_upload, sizeof(file_name_to_upload), 0);

		printf("file to upload on server is %s\n", file_name_to_upload);

		char file_location_tput[255] = "/home/user/Desktop/server_storage/";
		strcat(file_location_tput, file_name_to_upload);								// Concatination of first and second part of the file address

		char* final_location_tput = file_location_tput;
		FILE *file_per_tput = fopen(file_location_tput, "a");
		if(file_per_tput == NULL)
		{
			printf("File %s Cannot be opened file on server (errno:) %d\n", file_name_to_upload, errno);
		}
		else
		{
			bzero(file_size_tput, LENGTH); 
			int file_char_read_tput = 0;
			
			
			// char f_r[255];

			// int size;
			// unsigned NetInt;
			// char check[255] = "file sending starts";
			// recv(client_xyz -> socket_id,(char *)&NetInt,sizeof(long),0);
			// size=(int)ntohl(NetInt);
			// recv(client_xyz -> socket_id, &f_r, sizeof(f_r),0);
			// if(strcmp(f_r,check) == 0)
			// {
				
			// }
			// else
			// {
			// 	printf("Not synronize\n");
			// }

			//  for(; WAIT>1; WAIT = WAIT - 1)
		 //    {

		 //    }

			while((file_char_read_tput = recv(client_xyz -> socket_id, file_size_tput, LENGTH, 0)) > 0) 
			{
				 // for(; WAIT>1; WAIT= WAIT - 1)
			  //   {

			  //   }

			    int write_size = fwrite(file_size_tput, sizeof(char), file_char_read_tput, file_per_tput);
			    
			    // for(; WAIT>1; WAIT = WAIT - 1)
			    // {

			    // }

			    // char ack_s[255] = "file received" ;
			    // send(client_xyz -> socket_id, ack_s, sizeof(ack_s),0);
			    
			  
				if(write_size < file_char_read_tput)
			    {
			        error("File write failed on server (errno:) %d\n");
			    }
				bzero(file_size_tput, LENGTH);
				if (file_char_read_tput == 0 || file_char_read_tput != 512) 
				{
					break;
				}
			}
			if(file_char_read_tput < 0)
		    {
		        if (errno == EAGAIN)
	        	{
	                printf("recv() timed out.\n");
	            }
	            else
	            {
	                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
					exit(1);
	            }
        	}
			printf("File %s received from the client socket id %d!\n", file_name_to_upload, client_xyz -> socket_id);
			fclose(file_per_tput); 
			//pthread_mutex_unlock(&mutex);									// release the thread for this function
		}
	}

	// char exit_msg[100];
	// int check = recv(client_xyz -> socket_id, exit_msg, 100, 0);
	// if (strcmp(exit_msg,"exit\n")== 0)
	// {
	// 	printf("Client socket id %d disconnected from the server \n", client_xyz -> socket_id);
	// 	pthread_exit(NULL);
	// }
	pthread_exit(NULL);

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