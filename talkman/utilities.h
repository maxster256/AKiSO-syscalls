/*
 * utilities.h
 * Contains resources shared between the client and the server.
 * Manages imports
 */

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>    /* htons, htonl */
#include <netinet/in.h>   /* various functions and constants */
#include <sys/select.h>
#include <zconf.h>

// Constant value definitions
#define DEFAULT_SERVER_PORT 9877
#define MAX_CONNECTIONS 10          // max # of pending connections
#define MAX_MESSAGE_LEN 1024        // max # bytes sent between server & client

// Type redefinitions (for more clarity)
typedef struct sockaddr_in SA_in;
typedef struct sockaddr SA;