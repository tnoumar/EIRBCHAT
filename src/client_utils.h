#ifndef CLIENTUTILS_H_
#define CLIENTUTILS_H_
#include <stdio.h>

#define NUMFDS 2

//takes char * entry_STDIO and creates message struct
struct message message_parser(char *entree);

//closes server socket, and STDIO_FILENO
void disconnect(int server, int curr_fd);

//handles connection between server and client, returns sffd
int handle_connect(char *server_addr, char *port);

//print a text file in the terminal
void print_image(FILE *fptr);

#endif
