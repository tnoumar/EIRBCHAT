#include "client_utils.h"
#include "msg_struct.h"
#include "common.h"
#include "stdio.h"
#include "ctype.h"
#include "sys/stat.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "poll.h"
#include <signal.h>

struct message message_parser(char *entree)
{ //takes char * entry_STDIO and creates message struct
	const char *stop_char = "/quit\n";
	char *cmd;
	char nickname[NICK_LEN];
	char entry[MSG_LEN];
	strcpy(entry, entree);
	memset(nickname, '\0', NICK_LEN);
	cmd = strtok(entry, " ");
	struct message *wildcard_err = (struct message *)malloc(sizeof(struct message));

	wildcard_err->type = 3;
	wildcard_err->pld_len = 264; //sizeof message

	// /nick processing
	if (!strcmp(cmd, "/nick"))
	{
		memset(nickname, '\0', NICK_LEN);
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		//traitement nick
		int cpt = 0;
		while (cmd != NULL)
		{
			if (cpt > 1)
			{
				printf("no spaces or special characters in your nickname please\n");
				memset(nickname, '\n', NICK_LEN); //delete his nickname
				break;
			}
			else
			{
				strcat(nickname, cmd);
				cmd = strtok(NULL, " \\n");
				cpt++;
			}
		}

		//troubleshooting
		if ((isspace(nickname[0])) || cpt == 0 || strlen(nickname) > NICK_LEN)
		{
			printf("please enter a valid nickname\n ");
			return *wildcard_err;
		}

		else
		{
			// correct username
			//	printf("You've got the coolest username, %s\n", nickname);

			enum msg_type type = NICKNAME_NEW;

			//	int pld_len=2* sizeof(int) + 2*INFOS_LEN*sizeof(char );

			//remplissage de la structure
			wildcard_err->type = type;
			wildcard_err->pld_len = strlen(nickname);
			strcat(wildcard_err->nick_sender, " ");
			strcat(wildcard_err->infos, nickname);

			return *wildcard_err;
		}

	} //endif_nick

	else if (!strcmp(cmd, "/who\n"))
	{
		//process who
		enum msg_type type = NICKNAME_LIST;
		memset(wildcard_err, 0, sizeof(struct message));

		memset(wildcard_err->infos, '\0', INFOS_LEN);
		memset(wildcard_err->nick_sender, '\0', NICK_LEN);

		//remplissage de la structure
		wildcard_err->type = type;
		wildcard_err->pld_len = 0;
		strcat(wildcard_err->nick_sender, " ");
		strcat(wildcard_err->infos, " ");

		return *wildcard_err;
	}

	else if (!strcmp(cmd, "/whois"))
	{
		memset(nickname, '\0', NICK_LEN);
		cmd = strtok(NULL, " ,@/\\.&#"); //white space
		int cpt = 0;
		while (cmd != NULL)
		{
			if (cpt > 1)
			{
				printf("no spaces or special characters in the nickname please\n");
				memset(nickname, '\n', NICK_LEN); //delete his nickname
				break;
			}
			else
			{
				strcat(nickname, cmd);
				cmd = strtok(NULL, " ");
				cpt++;
			}
		}

		//troubleshooting
		if ((isspace(nickname[0])) || cpt == 0)
		{
			printf("please enter a valid nickname\n ");
			return *wildcard_err;
		}

		else
		{
			// correct username
			//OPTIMIZE USING WILDCARD ERR MSG FOR MEMORY PURPOSES
			enum msg_type type = NICKNAME_INFOS;
			memset(wildcard_err, 0, sizeof(struct message));

			//remplissage de la structure
			wildcard_err->type = type;
			wildcard_err->pld_len = strlen(nickname);
			strcat(wildcard_err->nick_sender, " ");
			strcat(wildcard_err->infos, nickname);

			return *wildcard_err;
		}
	}

	else if (!strcmp(cmd, "/msgall"))
	{
		memset(wildcard_err, 0, sizeof(struct message));
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		char buff[INFOS_LEN];
		memset(buff, '\0', INFOS_LEN);
		while (cmd != NULL)
		{
			strcat(buff, cmd);
			cmd = strtok(NULL, " ");
			strcat(buff, " ");
		}
		strcpy(wildcard_err->nick_sender, ""); //for the moment
		wildcard_err->type = 5;				   //BRDC
		wildcard_err->pld_len = strlen(buff);
		if (wildcard_err->pld_len > INFOS_LEN)
		{
			printf("Message size exceeded (127 letters allowed)\n");
			strcpy(wildcard_err->infos, "ERR_SIZE_EXCEEDED");
		}
		strcpy(wildcard_err->infos, buff);

		return *wildcard_err;
	}

	else if (!strcmp(cmd, "/join"))
	{
		memset(wildcard_err, 0, sizeof(struct message));
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		char buff[INFOS_LEN];
		memset(buff, '\0', INFOS_LEN);
		while (cmd != NULL)
		{
			strcat(buff, cmd);
			cmd = strtok(NULL, " ");
			strcat(buff, " ");
		}
		strcpy(wildcard_err->nick_sender, ""); //for the moment
		wildcard_err->type = 8;				   //BRDC
		wildcard_err->pld_len = strlen(buff);
		if (wildcard_err->pld_len > INFOS_LEN)
		{
			printf("Message size exceeded (127 letters allowed)\n");
			strcpy(wildcard_err->infos, "ERR_SIZE_EXCEEDED");
		}
		strcpy(wildcard_err->infos, buff);

		return *wildcard_err;
	}

	else if (!strcmp(cmd, "/msg"))
	{
		memset(wildcard_err, 0, sizeof(struct message));
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		char buff[INFOS_LEN];
		char nick[NICK_LEN];
		memset(nick, '\0', INFOS_LEN);

		memset(buff, '\0', INFOS_LEN);
		int cpt = 0;

		strcat(nick, cmd);
		cpt++; //we have a nickname

		cmd = strtok(NULL, "\n");
		wildcard_err->pld_len = strlen(buff);
		if (cmd == NULL)
		{
			printf("NO message detected(127 letters allowed)\n");
			strcpy(wildcard_err->infos, "NULL PAYLOAD");
			return *wildcard_err;
		}
		else
		{

			strncpy(buff, cmd, strlen(cmd));

			strcpy(wildcard_err->nick_sender, nick); //nicksender = dest_user
			memset(nick, '\0', NICK_LEN);
			wildcard_err->type = 4; //UNCST

			if (!strcmp(msg_type_str[4], "UNICAST_SEND"))
			{
				//to suppress warning
			}

			if (wildcard_err->pld_len > INFOS_LEN)
			{
				printf("Message size exceeded (127 letters allowed)\n");
				strcpy(wildcard_err->infos, "ERR_SIZE_EXCEEDED");
			}

			strcpy(wildcard_err->infos, buff);

			return *wildcard_err;
		}
	}
	else if (!strcmp(cmd, "/create"))
	{
		memset(nickname, '\0', NICK_LEN);
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		//traitement nick
		int cpt = 0;
		while (cmd != NULL)
		{
			if (cpt > 1)
			{
				printf("no spaces or special characters in room name please\n");
				memset(nickname, '\n', NICK_LEN); //delete his nickname
				break;
			}
			else
			{
				strcat(nickname, cmd);
				cmd = strtok(NULL, " \\n");
				cpt++;
			}
		}

		//troubleshooting
		if ((isspace(nickname[0])) || cpt == 0 || strlen(nickname) > NICK_LEN)
		{
			printf("please enter a valid roomname\n ");
			return *wildcard_err;
		}

		else
		{
			// correct username
			//	printf("You've got the coolest username, %s\n", nickname);

			enum msg_type type = MULTICAST_CREATE;

			//remplissage de la structure
			wildcard_err->type = type;
			wildcard_err->pld_len = strlen(nickname);
			strcat(wildcard_err->nick_sender, " ");
			strcat(wildcard_err->infos, nickname);

			return *wildcard_err;
		}
	}
	else if (!strcmp(cmd, "/quit"))
	{
		memset(nickname, '\0', NICK_LEN);
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		//traitement nick
		int cpt = 0;
		while (cmd != NULL)
		{
			if (cpt > 1)
			{
				printf("no spaces or special characters in room name please\n");
				memset(nickname, '\n', NICK_LEN); //delete his nickname
				break;
			}
			else
			{
				strcat(nickname, cmd);
				cmd = strtok(NULL, " \\n");
				cpt++;
			}
		}

		//troubleshooting
		if ((isspace(nickname[0])) || cpt == 0 || strlen(nickname) > NICK_LEN)
		{
			printf("please enter a valid roomname, like room101 \n ");
			return *wildcard_err;
		}

		else
		{
			// correct username
			//	printf("You've got the coolest username, %s\n", nickname);

			enum msg_type type = MULTICAST_QUIT;
			memset(wildcard_err, 0, sizeof(struct message));

			//remplissage de la structure
			wildcard_err->type = type;
			wildcard_err->pld_len = strlen(nickname);
			strcat(wildcard_err->nick_sender, " ");
			strcat(wildcard_err->infos, nickname);

			return *wildcard_err;
		}
	}
	else if (!strcmp(cmd, "/channel_list\n"))
	{
		enum msg_type type = MULTICAST_LIST;
		memset(wildcard_err, 0, sizeof(struct message));

		memset(wildcard_err->infos, '\0', INFOS_LEN);
		memset(wildcard_err->nick_sender, '\0', NICK_LEN);

		//remplissage de la structure
		wildcard_err->type = type;
		wildcard_err->pld_len = 0;
		strcat(wildcard_err->nick_sender, " ");
		strcat(wildcard_err->infos, " ");

		return *wildcard_err;
	}

	else if (!strcmp(cmd, "/send"))
	{
		//fill file info
		struct stat st;
		FILE *file2send = NULL;

		memset(wildcard_err, 0, sizeof(struct message));
		cmd = strtok(NULL, " ,@/\\.&#"); //delimeters ...
		char buff[INFOS_LEN];
		char nick[NICK_LEN];
		memset(nick, '\0', INFOS_LEN);
		memset(buff, '\0', INFOS_LEN);
		int cpt = 0;

		strcat(nick, cmd);
		cpt++; //we have a nickname

		cmd = strtok(NULL, "\n");
		wildcard_err->pld_len = strlen(buff);
		if (cmd == NULL)
		{
			printf("NO message detected(127 letters allowed)\n");
			strcpy(wildcard_err->infos, "NULL PAYLOAD");
			return *wildcard_err;
		}
		else
		{

			char filename[NICK_LEN];
			strncpy(buff, cmd, strlen(cmd));
			strncpy(filename, buff, strlen(buff) - 1);
			stat(buff, &st);
			int size = st.st_size;
			//opening fd for file to send
			//check if file exists
			if (NULL == (file2send = fopen(buff, "r")))
			{
				perror("Error opening file\n");
				wildcard_err->type = 11;
				wildcard_err->pld_len = 0;
				strcpy(wildcard_err->nick_sender, nick); //copying nick target to nicksender field
				return *wildcard_err;
			} //file exists
			else
			{
				strcpy(wildcard_err->nick_sender, nick); //nicksender = dest_user
				memset(nick, '\0', NICK_LEN);
				wildcard_err->type = 11; //REQUEST

				if (wildcard_err->pld_len > INFOS_LEN)
				{
					printf("Message size exceeded (127 letters allowed)\n");
					strcpy(wildcard_err->infos, "ERR_SIZE_EXCEEDED");
				}

				strcpy(wildcard_err->infos, buff);
				wildcard_err->pld_len = strlen(wildcard_err->infos);

				fclose(file2send);
				return *wildcard_err;
			} //endelse

		} //endelse
	}	  //elseif

	/*
	add elseifs to check other commands in the future
	 */

	// /quit processing
	else if (!strcmp(cmd, stop_char))
	{
		wildcard_err->type = 3;
		wildcard_err->pld_len = 0;
		strcpy(wildcard_err->infos, cmd);
		strcpy(wildcard_err->nick_sender, "");
		return *wildcard_err;
	}

	else if (!strcasecmp(cmd, "yes\n") || !strcasecmp(cmd, "no\n"))
	{
		fprintf(stdout, "yes or no");
		if (!strcasecmp(cmd, "yes\n"))
		{
			wildcard_err->type = 12;
		}
		else if (!strcasecmp(cmd, "no\n"))
		{
			wildcard_err->type = 13;
		}

		strcpy(wildcard_err->infos, cmd);
		strcpy(wildcard_err->nick_sender, "");
		wildcard_err->pld_len = strlen(wildcard_err->infos);
		return *wildcard_err;
	}

	else
	{
		printf("ERRRR! You need to type a command first. Start with /nick or go check how an IRC works...\n");
		wildcard_err->type = 3;

		strcpy(wildcard_err->infos, cmd);
		strcpy(wildcard_err->nick_sender, "");
		wildcard_err->pld_len = strlen(wildcard_err->infos);
		return *wildcard_err;
	}

	cmd = NULL;
	free(cmd);
	wildcard_err = NULL;
	free((void *)wildcard_err);
}

void disconnect(int server, int curr_fd)
{
	printf("disconnecting from server\n ");
	close(curr_fd);
	close(server);
}

int handle_connect(char *server_addr, char *port)
{
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int res_err;
	if ((res_err = getaddrinfo(server_addr, port, &hints, &result)) != 0)
	{

		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res_err));
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
		{
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			break;
		}
		close(sfd);
	}
	if (rp == NULL)
	{
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

void print_image(FILE *fptr)
{
	char read_string[MSG_LEN];

	while (fgets(read_string, sizeof(read_string), fptr) != NULL)
		printf("%s", read_string);
}
