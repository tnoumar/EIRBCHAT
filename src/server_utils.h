#ifndef SERVERUTILS_H_
#define SERVERUTILS_H_

#include "sys/types.h"
#include "stdio.h"
//prints connection time of nick user
void time_conn(char *dst);
struct nickdb;
struct clientlist;
struct message;
struct room;
typedef struct clientlist *node; //Define node as pointer of data type struct clientlist
typedef struct nickdb *user;     //Define user as pointer of data type struct nickdb
typedef struct room *room_ptr;   //Define room as pointer of data type struct room

node createGenesis();
user createUser();
room_ptr createRoom();
node addNode(node *head, u_int fd, u_short port, char *addr);
user addNick(user *head, u_int fd, char *nick);
room_ptr addRoom(room_ptr *rooms, u_int fd, char *name);
void suppr(node *head, int fd);
void supprUser(user *head, int fd);
void supprRoom(room_ptr *rooms, char *name);
void afficheList(node *deb);
void afficheUser(user *deb);
void afficheRoom(room_ptr *deb);
char *puts_afficheruser(user *deb, char *dest);
char *puts_afficherrooms(room_ptr *rooms, char *dest, int clientfd);
struct clientlist *adresselist(node *deb, u_int fd);
void detruit_liste(node deb);
int count(node *head);
char *joinRoom(room_ptr *rooms, user *nicknames, char *roomname, int clientfd);
int countRoom(room_ptr *room);
int countUser(user *head);
void print_image(FILE *fptr);
int echo2client(struct message msg, int clientfd);
int exist_user(char *nick, user *nicknames);
int exist_room(char *name, room_ptr *rooms);
int quitRoom(room_ptr *rooms, int clientfd, char *roomname);

char *getUserCurrentRoom(char *nick, user *nicknames, char *currentroom);
char *getnick(int fd, user *nicknames, char *dest);
void proc_multicast(struct message *msg, int type, int clientfd); // create salon
void proc_nick(struct message *msg, int type, int clientfd);
void proc_who(struct message *msg, int type, int clientfd);
void main_thread(int sockfd);
int handle_bind(const char *port);
#endif