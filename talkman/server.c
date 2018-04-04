/*
 * server.c
 * A server for the talkman chat program.
 * It creates a connection with client, and then passes the messages sent by each client
 * to every other client that is currently connected to the server.
 * Also, a simple log of all the actions is prepared.
 * Function select() handles multiple clients.
 */

#include "utilities.h"

void run_server(int* serv_fd, fd_set* cli_fds, fd_set* main_fd, int* max_fd, SA_in* cli_addr);
void configure_server(fd_set *main_fd, fd_set *cli_fds, int *serv_fd, SA_in *serv_addr);
void add_new_client(int id, fd_set *main_fd, int *max_fd, int serv_fd, SA_in *cli_addr);
void handle_client(int id, fd_set *main_fd, int serv_fd, int max_fd);
void add_message_to_log(char *msg);


int main(int argc, char** argv) {
	
	// Local variables
	fd_set main_fd, cli_fds; 		//file descriptor sets for main server and clients
	int serv_fd = 0, max_fd; 		//fd for server and max fd for select
	SA_in serv_addr, cli_addr; 		//address structs for server and clients

	// Set all of the necessary server properties before the connection is made
	configure_server(&main_fd, &cli_fds, &serv_fd, &serv_addr);

	// Server will be the highest possible fd until the next one will connect
	max_fd = serv_fd;

	// Begin server processing
	run_server(&serv_fd, &cli_fds, &main_fd, &max_fd, &cli_addr);

	return 0;
}

void run_server(int* serv_fd, fd_set* cli_fds, fd_set* main_fd, int* max_fd, SA_in* cli_addr) {

	// An infinite loop used to process the connections made by the clients,
	// When something is happening (i.e. the client is sending a message),
	// the loop is being run through and proper methods are invoked

	// About select
	// select() allows a program to monitor multiple file descriptors, waiting until one or more
	// of the file descriptors become "ready" for some class of I/O operation (e.g., input possible).
	// A file descriptor is considered ready if it is possible to perform the corresponding I/O operation
	// (e.g., read(2)) without blocking.

	// select() uses a timeout that is a struct timeval (with seconds and microseconds)
	// select() may update the timeout argument to indicate how much time was left

	int i; // Controls the loop
	for (;;) {
		*cli_fds = *main_fd;

		// When client activity occurs, store that client's fd
		// in cli_fds for processing
		select(*max_fd+1, cli_fds, NULL, NULL, NULL);

		// Cycle through clients and see which one had activity
		for (i = 0; i < *max_fd + 1; i++)
		{
			// If the current client is the active one
			if (FD_ISSET(i, cli_fds))
			{
				// Check if it is a new client connecting...
				if (i == *serv_fd)
					add_new_client(i, main_fd, max_fd, *serv_fd, cli_addr);
				else //...or an existing client sending a msg
					handle_client(i, main_fd, *serv_fd, *max_fd);
			}
		}
	}
}

void configure_server(fd_set *main_fd, fd_set *cli_fds, int *serv_fd, SA_in *serv_addr) {

	// Set up of the server before the connections are made.
	// Creates the socket and ensures that it can handle multiple connections.
	// Then, it binds the socket and the server address, and allows the server to listen.

	// Settings
	int reusability = 1; 			//TCP address reusability option

	// Prepare to accept a connection
	FD_ZERO(main_fd); // Clears the fd's
	FD_ZERO(cli_fds);

	// Prepares the stucture which stores the server address
	bzero(serv_addr, sizeof(SA_in)); 		//also sets unused bytes to 0 (using string.h)
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(DEFAULT_SERVER_PORT);
	serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	// Try to create a TCP socket
	*serv_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Allow multiple clients to connect by reusing the server
	setsockopt(*serv_fd, SOL_SOCKET, SO_REUSEADDR, &reusability, sizeof(reusability));

	// Bind socket and server address together
	bind(*serv_fd, (SA *)serv_addr, sizeof(SA));

	// Make sure not to listen to more than the maximum amount of connections specified in the programme configuration
	listen(*serv_fd, MAX_CONNECTIONS);

	// Store server fd in main fd set
	FD_SET(*serv_fd, main_fd);

	// Print the info for the system admin
	printf("Server is running on port: %d\n", DEFAULT_SERVER_PORT);
	printf("Awaiting clients...\n");
	printf("You can stop the server any time by using CTRL + c.\n");
}

void add_new_client(int id, fd_set *main_fd, int *max_fd, int serv_fd, SA_in *cli_addr) {

	// If an incoming connection is requested, the client's information is received
	// and added to the main fd_set, which allows to communicate through the server.

	// Accepts a new connection
	socklen_t addrlen = sizeof(SA_in);
	int new_fd = accept(serv_fd, (SA*)cli_addr, &addrlen);

	// Make sure that the client has been connected to the server properly.
	if(new_fd < 0) {
		fprintf(stderr, "There's been an error while connecting the clients to the server.\n");
		exit(1);
	}
	else {
		// Add new client to our file descriptors' set
		FD_SET(new_fd, main_fd);

		// If new client has the highest fd, save it for use for the select() later
		if(new_fd > *max_fd)
			*max_fd = new_fd;

		// Log client connection to server
		char activity[75];
		snprintf(activity, sizeof(activity),
			"New client connected %s on port %d\n",
			inet_ntoa(cli_addr->sin_addr), ntohs(cli_addr->sin_port));
		add_message_to_log(activity);
	}
}

void handle_client(int id, fd_set *main_fd, int serv_fd, int max_fd) {

	// If a client whih is already connected performs any action, handle it.
	// If a client has disconnected, remove him from the fd_set;
	// Otherwise, if a client sends a message, read it and relay it to the other clients.

	int n; 							//number of bytes received
	int j; 							//used for loops
	char msg_buf[MAX_MESSAGE_LEN];

	// The client is disconnected if there are no messages received
	if ((n = read(id, msg_buf, MAX_MESSAGE_LEN)) < 1) {
		char activity[24];
		snprintf(activity, sizeof(activity), "Socket %-2d disconnected\n", id); // Log the fact that the client has been disconnected
		add_message_to_log(activity);

		close(id); // Close the connection with ex-client
		FD_CLR(id, main_fd);
	}
	else {
		// A message was received
		int logged = 0; // Track if the message logged already

		// Send the message to all clients except the one who sent it
		for(j = 0; j < max_fd + 1; j++) {

			// Send the message to the other clients
			if (FD_ISSET(j, main_fd)) // Make sure that the other client exists
				if (j != serv_fd && j != id) // Ignores the server and the sender while sending message to the others
					write(j, msg_buf, n);

			if (logged == 0) {

				// Log messages sent by clients only once. This code here makes sure that even if only one
				// client is connected, sent messages are still recorded.
				msg_buf[n] = '\0'; //for logging
				add_message_to_log(msg_buf);
				logged = 1;
			}
		}
	}
}

void add_message_to_log(char *msg) {

	// Logs the message that was sent to the server long.
	// The log includes the time when the message was sent.
	// Also, the log is printed to the server console.
	// If the log file does not exist, it will be created.

	// Local variables
	FILE *log_file = fopen("serverlog.txt", "a"); // log file
	time_t raw_time; 							  // time data
	struct tm *time_info; 						  // structured time data

	// Formats the time and stores as a char array
	time(&raw_time);
	time_info = localtime(&raw_time);

	char *time_str = asctime(time_info); 		  // sets the formatted time
	time_str[strlen(time_str)-1] = '\0';

	// Prints to the server console and logs it into the file
	printf("[%s]%s", time_str, msg);
	fprintf(log_file, "[%s]%s", time_str, msg);

	// Closes the connection
	fclose(log_file);
}
