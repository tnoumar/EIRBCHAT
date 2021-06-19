
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include <sys/socket.h>
#include <unistd.h>
#include "poll.h"
#include "pthread.h"
#include "msg_struct.h"
#include <time.h>
#include "common.h"
#include "server_utils.h"
//clean logo for my server
char *filename = "eirblogo.txt";
FILE *fptr = NULL;

//defintion de la liste chainee ou seront stockés les infos de chaque client

node client_list;
user nicknames;
room_ptr rooms;
struct message wilcard;

//returns user.fd if user exists or -1 else

int main(int argc, char **argv)
{
  //initializing main datatypes
  rooms = addRoom(&rooms, 0, "MAINROOM");
  client_list = addNode(&client_list, 0, 0, "root");

  nicknames = addNick(&nicknames, 0, "EIRBADMIN (default)");

  //file descriptor for my logo
  if ((fptr = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "error opening %s\n", filename);
    return 1;
  }
  print_image(fptr);
  fclose(fptr);

  if (argc != 2)
  {
    perror("usage : exec <port>");
  }

  //affectation du port
  int sfd;
  //bindin'
  sfd = handle_bind(argv[1]);
  printf("BINDIN' ....");
  if ((listen(sfd, SOMAXCONN)) != 0)
  {
    perror("listen()\n");
    exit(EXIT_FAILURE);
  }

  //they see me pollin'
  int ndfs = MAXCLI;
  int poll_delay = -1; //delay for poll
  struct pollfd fds[ndfs];
  pthread_t pth_t[MAXCLI]; //creation de MAXCLI threads pour chaque fd dispo
  memset(fds, 0, ndfs * sizeof(struct pollfd));
  for (int i = 1; i < MAXCLI; i++)
  {
    fds[i].fd = -1;
  }
  fds[0].fd = sfd;
  fds[0].events = POLLIN;

  while (1)
  { //mainwhile
    switch (poll(fds, MAXCLI, poll_delay))
    {
    case 0:
      printf("timeout...\n");
      continue;
    case -1: // failed
      perror("poll fail...\n");
      continue;
    default: // Succeed
    {
      for (int i = 0; i < MAXCLI; i++)
      {
        if (i == 0 && (fds[i].revents & POLLIN))
        {
          struct sockaddr_in client_addr;
          socklen_t size_addr = sizeof(struct sockaddr_in);
          printf("start accepting...\n");
          int client_fd = -1;
          if (-1 == (client_fd = accept(sfd, (struct sockaddr *)&client_addr, &size_addr)))
          {
            perror("Error while accepting");
            return 0;
          }

          int k = 1;
          while (fds[k].fd != -1)
          {
            k++;
          }
          fds[k].fd = client_fd;
          fds[k].revents = POLLIN;
          fds[0].revents = 0;

          char *current_cliaddr = inet_ntoa(client_addr.sin_addr);
          u_short current_cliport = ntohs(client_addr.sin_port);
          printf("nouveau client connecté[%s:%d]\n", current_cliaddr, current_cliport);
          memset(&wilcard, 0, sizeof(struct message)); //tofree, this will be the welcome message
          //filling wildcard
          wilcard.type = 3; //echo
          strcpy(wilcard.nick_sender, "EIRBADMIN");
          char *wlcmsg = "[EIRBCHAT]  : please login with /nick <your pseudo>";
          strcpy(wilcard.infos, wlcmsg);
          wilcard.pld_len = sizeof(wlcmsg);

          //sending welcome message to client

          if (send(client_fd, &wilcard, 264, 0) < 0)
          {
            perror("error delivering welcome message to client");
          }
          //client_list=createGenesis(); //initializing our client linked list
          client_list = addNode(&client_list, client_fd, current_cliport, current_cliaddr); //adding each connected client
          afficheList(&client_list);
          count(&client_list);
        }
        else if (i != 0 && (fds[i].revents & POLLIN))
        {
          // Affichage
          printf("socket server is %d\n", sfd);
          printf("client socket is %d\n", fds[i].fd);

          pthread_create(&pth_t[i], NULL, (void *)main_thread, (void *)(intptr_t)fds[i].fd);
          pthread_detach(pth_t[i]); //thread memory freed
        }

        else if (i != 0 && (fds[i].revents & (POLLHUP | POLLERR)))
        {
          // close connection with client when required
          printf(" \n client deconnecté fd : %d\n", fds[i].fd);
          suppr(&client_list, fds[i].fd); //suppression du node déconnecté
          int l = i + 1;
          while (l > 0)
          {
            supprUser(&nicknames, fds[i].fd);
            l--;
          }
          //print the port of the disconnected client
          close(fds[i].fd);
          fds[i].fd = -1;
          // close socket fd
        }
      }
    } //default
    break;
    } //switch
  }
  //server shutdown
  for (int i = 0; i < MAXCLI; i++)
  {
    assert(pthread_cancel(pth_t[i]) == 0);
    printf("killing threads \n");
    //killing session threads
  }
  free((void *)&wilcard);
  free(nicknames);
  free(client_list);
  close(sfd);
  detruit_liste(client_list);
  assert(client_list == NULL);
  return EXIT_SUCCESS;
}