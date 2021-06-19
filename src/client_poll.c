#include "ctype.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "poll.h" 
#include "common.h"
#include"signal.h"
#include "msg_struct.h"
#include"client_utils.h"
// const char * stop_char="/quit\n";

struct pollfd fds_c[NUMFDS];



const char * stop_char="/quit\n";



int main(int argc, char** argv) {

	char volatile_nick[NICK_LEN];
	if (argc !=3)
	{
	perror("usage: exec <server> <port>");
	}

	int sfd;
	sfd = handle_connect(argv[1], argv[2]);
	memset(fds_c, 0 ,NUMFDS*sizeof(struct pollfd));
	int flagstop=1;
	int poll_delay=-1;
	fds_c[0].fd=STDIN_FILENO; //gerer l'ecriture sur stdin
	fds_c[0].revents=0;
	fds_c[0].events=POLLIN;
	fds_c[1].fd=sfd; //gerer les donnees envoyees par le serveur
	fds_c[1].revents=0;
	fds_c[1].events=POLLIN;
	//assimilate signint to a disconnection .pending.. raises at execution
	//signal(SIGINT, disconnect(sfd, STDIN_FILENO));
		
	while (1 & flagstop)
	{
		
		switch (poll(fds_c, NUMFDS, poll_delay ))
		{
		case 0:
			printf("timeout...");
			continue;
		

			case -1:
			printf("poll fail...");
			continue;
		default: //poll succeeded

				//sending chars to server
				if (fds_c[0].revents & POLLIN )
				{
					fprintf(stdout,"\n [CLIENT]:  \t");
					int n;
					
					char buff[MSG_LEN]; //raw in stdin
					memset(buff, 0, MSG_LEN);
					// Getting message from stdin
					n = 0;
					struct message msg2send;
					memset(&msg2send, 0, sizeof(struct message));
					fflush(STDIN_FILENO); //et ouais la chasse
					while ((buff[n++] = getchar()) != '\n') {} // trailing '\n' will be sent
					
					if (!strcmp(buff, "/quit\n")) //dumbo wants to quit
						{
						
						msg2send=message_parser(buff);

						if (send(fds_c[1].fd, &msg2send, sizeof(msg2send), 0) <= 0 ) 
							{

							}
							memset(&msg2send, 0, sizeof(struct message)); //cleaning 
							memset(buff, 0, MSG_LEN);//cleaning
						printf("You will be disconnected\n");
						
						
						disconnect(fds_c[0].fd, sfd);
						
						flagstop=0; //kill the while
						
						}

					else
						{ 
							msg2send=message_parser(buff);
							//Sending message 
							if (send(fds_c[1].fd, &msg2send, sizeof(msg2send), 0) <= 0 ) 
							{
								perror("Cant send data to server...");
							}

							memset(&msg2send, 0, sizeof(struct message)); //cleaning 
							memset(buff, 0, MSG_LEN);//cleaning
						}
						
				} //end_if pollin stdin
				//receiving chars from server
				else if (fds_c[1].revents & POLLIN )
				{	//meanwhile handles echo msg from server
					struct message msg_rcvd;
					memset(&msg_rcvd, 0, sizeof(struct message));
					printf("\033[1;31m");	
					fprintf(stdout, "SERVER -> : \t");
					  printf("\033[0m");
					char buf_fromserver[MSG_LEN];
					memset(buf_fromserver, '\0', MSG_LEN);
					int received_data;
					  if ((received_data=read(fds_c[1].fd, &msg_rcvd, sizeof(struct message))) <= 0) {
					    perror("Error while receiving data; SERVER UNREACHABLE \n");
						flagstop=0;
					  }
					 				  	
						fprintf(stdout, "%s\n", msg_rcvd.infos);
						
						
					}

				
				
				
				// closing fds upon disconnection or error
				
		
			break;
		}//switch
	}//while
				if (fds_c[1].revents &( POLLHUP | POLLERR))
				{
					printf("disconnecting from the server ... \n");
					close(fds_c[1].fd);
					
				}
				
				 if (fds_c[0].revents &( POLLHUP | POLLERR))
				{
					printf("turning off STDIN ... \n");
					close(fds_c[0].fd);
				}


	close(sfd);
	exit(0);
}










