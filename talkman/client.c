/*
 * client.c
 * A client for the talkman chat program.
 * Makes it possible for the users to connect to the talkman server.
 * Once connected, the messages can be sent to the other clients, which are also connected to the server.
 */

#include "utilities.h"

void client_activity(fd_set *cli_fds, fd_set *main_fd, int *max_fd, int *sock_fd, char *name);
void create_new_connection(char *ip_addr, fd_set *main_fd, fd_set *cli_fds, int *sock_fd, SA_in *serv_addr);
void send_message(char *name, int sock_fd);
void recv_msg(int sock_fd);


int main(int argc, char** argv)
{
	int sock_fd, max_fd; 		// fd for this client's connection and max fd value
	fd_set main_fd, cli_fds; 	// fd sets for main server and clients
	SA_in serv_addr; 			// address struct for server
	char* name; 				// username

	if (argc != 3) {
		printf("usage: client <IP address> <username>\n");
		exit(1);
	}

	name = argv[2]; 			// Sets user's nickname for use later
	
	// Creates a new connection to server
	create_new_connection(argv[1], &main_fd, &cli_fds, &sock_fd, &serv_addr);

	// Sends greetings to the user
	printf("Successfully connected to %s.\n", argv[1]);
	printf("You may begin typing at any time.\n\n");

	// Runs the client's loop which will handle message sending and receiving
	client_activity(&cli_fds, &main_fd, &max_fd, &sock_fd, name);

	// Closes the connection made by the user
	close(sock_fd);
	return 0;
}

/**
 * Begin running an infinite loop to process client activity, both from the
 * user and other clients. Sends a message if it's the user, and otherwise
 * receives a message if activity is from another client.
 **/
void client_activity(fd_set *cli_fds, fd_set *main_fd, int *max_fd, int *sock_fd, char *name) {

	// Runs an infinite loop which processes what's happening
	// from the user as well as other clients.

	// A message is sent, if our client decided to do so.
	// If someone else sends a message, we receive it.

	*max_fd = *sock_fd;
	
	int iterator;
	for (;;) {
		// Awaits for the activity, either from us or from the other clients
		*cli_fds = *main_fd;
		select(*max_fd+1, cli_fds, NULL, NULL, NULL);

		// Iterates through clients and processes all of the activity
		for (iterator = 0; iterator < *max_fd + 1; ++iterator)
			if (FD_ISSET(iterator, cli_fds)) {            // Checks if the clients exists
                if (iterator == 0)                        // Send the message if it's from us
                    send_message(name, *sock_fd);
                else                                      // Receive the message if it comes from somebody else
                    recv_msg(*sock_fd);
            }
	}
}

void create_new_connection(char *ip_addr, fd_set *main_fd, fd_set *cli_fds, int *sock_fd, SA_in *serv_addr) {

	// Establish a new TCP socket and initalize the connection with the server
	*sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Configure the structure of the server address which is used for the connection
	bzero(serv_addr, sizeof(SA_in));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(DEFAULT_SERVER_PORT);
	inet_pton(AF_INET, ip_addr, &(serv_addr->sin_addr));
	
	// Connect to the server
	connect(*sock_fd, (SA*)serv_addr, sizeof(SA));

	// Clear fds before being added to the main fd_set
	FD_ZERO(main_fd);
	FD_SET(0, main_fd);
	FD_ZERO(cli_fds);
	FD_SET(*sock_fd, main_fd);	
}

void send_message(char *name, int sock_fd) {

	// Sends a message through the socket to the server
	// (and to the other clients, as well)

	char msg_buf[MAX_MESSAGE_LEN];

	// Get input from user
	fgets(msg_buf, MAX_MESSAGE_LEN, stdin);

	char full_msg[MAX_MESSAGE_LEN] = {'0'};
	snprintf(full_msg, sizeof(msg_buf), "%s: %s", name, msg_buf);

	write(sock_fd, full_msg, strlen(full_msg));
}


void recv_msg(int sock_fd) {

	// Receives the message from the other clients through the socket

	// Reads the  message from the socket
	char msg_buf[MAX_MESSAGE_LEN];
	int n = read(sock_fd, msg_buf, MAX_MESSAGE_LEN); 		// Number bytes received
	msg_buf[n - 1] = '\0'; 									// Prevents extra bytes from going through

	// Print the message
	printf("%s\n", msg_buf);
} //end recv_msg
