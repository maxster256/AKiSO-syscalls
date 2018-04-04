// Socketlib.h

int server_tcp_socket(char *hn, int port);
int accept_tcp_connection(int s);
int request_tcp_connection(char *hn, int port);
