
#include"server_utils.h"
#include"sys/stat.h"
#include"time.h"
#include"sys/types.h"
#include"common.h"
#include"msg_struct.h"
#include"stdio.h"
#include <netdb.h>
#include <unistd.h>
#include"string.h"
#include"stdlib.h"
#include"assert.h"
#include"sys/socket.h"

void time_conn (char * dst)
{
  time_t rawtime;
  struct tm * timeinfo;
  char buff[80];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strftime(buff, 80, "%Y/%m/%d@%H:%M", timeinfo);
  strcpy(dst, buff);
}
int handle_connect( char * server_addr, char * port ) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int res_err;
	if ((res_err=getaddrinfo(server_addr, port, &hints, &result)) != 0) {
	
		 fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res_err	));
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////// LINKED LISTS //////////////////////////////////////////////////
struct nickdb
{
  int fd;
  char nick[NICK_LEN];
  char current_salon[NICK_LEN];
  struct nickdb *next;
};
struct transfer_log{
    int fd_s;
    int fd_r;
    char filename[NICK_LEN];
};
struct room{
  char name[NICK_LEN];
  u_short active_users[1024]; // boolean tab
  struct room* next;
};

struct clientlist {
   u_int fd;
   u_short port;
   char * addr;
   char  time_connection[80];
   struct clientlist *next;
};

typedef struct clientlist *node; //Define node as pointer of data type struct clientlist
typedef struct nickdb *user; //Define user as pointer of data type struct nickdb
typedef struct room *room_ptr; //Define room_ptr as pointer of data type struct room

node client_list;
user nicknames;
room_ptr rooms;
struct message wilcard;

node createGenesis(){
    node temp; // declare a node
    temp = (node)malloc(sizeof(struct clientlist)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}

user createUser(){
    user temp; // declare a node
    temp = (user)malloc(sizeof(struct nickdb)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}
room_ptr createRoom(){
    room_ptr temp; // declare a node
    temp = (room_ptr)malloc(sizeof(struct room)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}

node addNode(node *head, u_int fd, u_short port, char* addr){
    node temp,p;// declare two nodes temp and p
    temp = createGenesis();//createGenesis will return a new node with data = value and next pointing to NULL.
    temp->fd = fd; 
    temp->port=port;
    
    temp->addr=addr;

  
    time_conn(temp->time_connection);
    // add element's value to data part of node
    if(head == NULL){
        head = (node *)temp;     //when linked list is empty
    }
    else{
        p  =(node) head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return *head;
}

user addNick(user *head, u_int fd, char * nick){
    user temp,p;// declare two nodes temp and p
    temp = createUser();//createGenesis will return a new node with data = value and next pointing to NULL.
    temp->fd = fd; 
    strcpy(temp->nick, nick);
    strcpy(temp->current_salon,"MAINROOM");
    if (fd!=0)
    {
      if(!strcmp(joinRoom(&rooms, &nicknames, "MAINROOM", fd),"__FAIL__")){
      printf("Can't add user to room\n" );
    }
    
    }
    
    // add element's value to data part of node
    if(head == NULL){
        head =(user *) temp;     //when linked list is empty
    }
    else{
        p  = (user)head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return *head;
}

room_ptr addRoom(room_ptr *rooms, u_int fd, char * name){
    room_ptr temp,p;// declare two nodes temp and p
    temp = createRoom();//createGenesis will return a new node with data = value and next pointing to NULL.
    strcpy(temp->name, name);
    for (int i = 0; i < 1024; i++)
    {//on parcourt le tableau des utilisateurs connectes
        if (i==fd)
        {
            temp->active_users[i]=1;
        }
        temp->active_users[i]=0;
        
    }
    
    // add element's value to data part of node
    if(rooms == NULL){
        rooms =(room_ptr*) temp;     //when linked list is empty
    }
    else{
        p  = (room_ptr)rooms;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return *rooms;
}

void suppr(node *head, int fd)
{
    // Store head node
    node temp = (node)head, prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->fd == fd)
    {
        head = (node*)temp->next;   // Changed head
        free(temp);               // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->fd != fd)
    {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL) return;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp);  // Free memory
}
void supprUser(user *head, int fd)
{
    // Store head node
    user temp = (user)head, prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->fd == fd)
    {
        head =(user*) temp->next;   // Changed head
        free(temp);               // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->fd != fd)
    {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL) return ;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp);  // Free memory
}

void supprRoom(room_ptr* rooms, char *name)
{
    // Store head node
    room_ptr temp = (room_ptr)rooms, prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && !strcmp(temp->name, name))
    {
        temp =(room_ptr) temp->next;   // Changed head
        free(temp);               // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && strcmp(temp->name, name)!=0)
    {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL) return ;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp);  // Free memory
}

void afficheList(node *   deb) {
  struct clientlist* paux = (node)deb;
  // on prend un pointeur auxiliaire pour ne pas perdre l'adresse de début de la liste
  while(paux != NULL) {
    printf(" fd : %d \t", paux->fd);
    printf("port : %d \t", paux->port);
    printf(" addr :%s \t", paux->addr);
    printf("conn_time :%s \t", paux->time_connection);
     printf("\n");
    paux = paux->next;
  }
  printf("\n");
}

void afficheUser(user *   deb) {
  struct nickdb* paux = (user)deb;
  // on prend un pointeur auxiliaire pour ne pas perdre l'adresse de début de la liste
  while(paux != NULL) {
    printf(" fd : %d \t", paux->fd);
    printf(" nick :%s \t", paux->nick);
    printf(" ROOM :%s \t", paux->current_salon);

     printf("\n");
    paux = paux->next;
  }
  printf("\n");
}

void afficheRoom(room_ptr*   deb) {
  struct room* paux = (room_ptr)deb;
  // on prend un pointeur auxiliaire pour ne pas perdre l'adresse de début de la liste
  while(paux != NULL) {
    printf("room name is  : %s \t", paux->name);
     
    paux = paux->next;
  }
  printf("\n");
}

char * puts_afficheruser(user *deb, char *dest){
  struct nickdb* paux = (user)deb;
  char buff[INFOS_LEN];
  memset(buff, '\0', INFOS_LEN);
  int cnt=0;

  strcat(buff, "ONLINE USERS ARE: \n");
  while(paux != NULL) {
    if (cnt<1)
    {
      cnt++;
      
      paux = paux->next;

    }
    char linebuff[NICK_LEN*2];
    char nick[NICK_LEN];
    memset(nick, '\0', NICK_LEN);
    assert(paux->nick);
    printf("paux->nick is : %s\n",paux->nick);
    strncpy(nick, paux->nick, NICK_LEN  );
    printf(" nick to send is : %s\n", nick);
    sprintf(linebuff, "\t \t \t \t  - %s \n", nick);
    strcat(buff, linebuff);
    memset(nick, '\0', NICK_LEN); //cleaning for next iteration
    memset(linebuff, '\0', 2*NICK_LEN); //cleaning for next iteration
  
    paux = paux->next;
  }
  strcpy(dest, buff);
  return dest;
}
char * puts_afficherrooms(room_ptr *rooms, char *dest, int clientfd){
  struct room* paux = (room_ptr)rooms;
  char buff[INFOS_LEN];
  char current[NICK_LEN];
  getnick(clientfd,&nicknames, buff);
getUserCurrentRoom(buff,&nicknames, current);
printf("current is %s\n", current);
  memset(buff, '\0', INFOS_LEN);
  int cnt=0;

  strcat(buff, "Available rooms are: \n");
  while(paux != NULL) {
    if (cnt<1)
    {
      cnt++;
      
      paux = paux->next;

    }
    char linebuff[NICK_LEN*2];
    char name[NICK_LEN];
    memset(name, '\0', NICK_LEN);
    assert(paux->name);
   
    
    printf("paux->name is : %s\n",paux->name);
    strncpy(name, paux->name, NICK_LEN  );
     printf(" nick to send is : %s\n", name);
     if (!strcmp(name, current))
    { //if it is the current room
    sprintf(linebuff, "\t \t \t current: - %s \n", name);
    
    }else
    {
    sprintf(linebuff, "\t \t \t \t  - %s \n", name);
    }
    
    strcat(buff, linebuff);
    memset(name, '\0', NICK_LEN); //cleaning for next iteration
    memset(linebuff, '\0', 2*NICK_LEN); //cleaning for next iteration
  
    paux = paux->next;
  }
  strcpy(dest, buff);
  return dest;
}

char * getUserCurrentRoom(char *nick, user* nicknames, char * currentroom){
  user head;
  
  for ( head = (user)nicknames; head != NULL; head=head->next){
  
  if (!strcmp(head->nick, nick))
  {
    printf("User %s current room is %s\n", head->nick, head->current_salon);
    strcpy(currentroom, head->current_salon);
    return currentroom;
  }
  continue;
    
    
    
}
head=NULL;
free(head);
printf("User has no room error\n");
return NULL;
}

struct clientlist * adresselist(node *deb, u_int fd) {
  struct clientlist* head;
  // on prend un pointeur auxiliaire pour ne pas perdre l'adresse de début de la liste
  for ( head = (node)deb; head != NULL; head=head->next){
  
  if (head->fd==fd)
  {
    printf("adresse list succeeded %s\n", head->time_connection);
    return head;
  }
  continue;
    
    
    
}
head=NULL;
free(head);
printf("adresse list not found");
return NULL;
}



void detruit_liste(node deb) {
 struct clientlist*  paux = deb;
  // on prend un pointeur auxiliaire pour ne pas perdre l'adresse de début de la liste
  while(paux != NULL) {
      struct clientlist* tmp = paux;
      free(paux);
     paux = tmp->next;
  }
  deb = NULL;
}

//node counter for debug
int count(node * head) {
    int cnt = 0;
    node current_node =(node) head;
    while ( current_node != NULL) {
        cnt++;
        current_node = current_node->next;
      
    }
printf("\n Total no. of nodes is %d \n",cnt);
return cnt;
}

int countUser(user * head) {
    int cnt = 0;
    user current_node =(user) head;
    while ( current_node != NULL) {
        cnt++;
        current_node = current_node->next;
      
    }
printf("\n Total no. of nodes is %d \n",cnt);
return cnt;
}
int countRoom(room_ptr * room) {
    int cnt = 0;
    room_ptr current_node =(room_ptr) room;
    while ( current_node != NULL) {
        cnt++;
        current_node = current_node->next;
      
    }
printf("\n Total no. of nodes is %d \n",cnt);
return --cnt;
}

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////// LOGO //////////////////////////////////////////////////


void print_image(FILE *fptr)
{
    char read_string[MSG_LEN];
 
    while(fgets(read_string,sizeof(read_string),fptr) != NULL)
        printf("%s",read_string);
}

int echo2client(struct message msg, int clientfd ){
    if (send(clientfd,  (void *) &msg, sizeof(msg), 0)<0)
    {
      perror("failed to deliver echo to user\n");
      return 1;
    }
    return 0;


}
int quitRoom(room_ptr* rooms, int clientfd, char* roomname){
 //user quits room

  room_ptr head;
  for ( head = (room_ptr)rooms; head != NULL; head=head->next){
  
  if (!strcmp(head->name, roomname)) //we found the roomname
  {
    head->active_users[clientfd]=0;
    fprintf(stdout, "user quitted room %s\n", roomname);
    if (strcmp(head->name, "MAINROOM")!=0 ) //dont delete mainroom
    {
        int active_users;
        for (int i = 1; i <=1024; i++)
        {
          active_users+=head->active_users[i];
        }
        if (active_users==0)
        {
          supprRoom(rooms,roomname);
          printf("deleting room %s\n", roomname);
        }
        
    }
    
    return 0 ;
  }
  continue;
}
head=NULL;
free(head);
printf("adresse list not found");
return 1;
}
char* joinRoom(room_ptr* rooms,user * nicknames, char* roomname,int clientfd){
  room_ptr head;
  int res=1;
  //connect him to wanted salon traverser les rooms disponibles d'abord
  int cnt=0;
  for ( head = (room_ptr)rooms; head != NULL; head=head->next)
  {
    if (cnt<1)
    {
      cnt++;
      continue;
    }
    else
    {
      if (!strncmp(head->name, roomname, strlen(roomname)-2))
      {
          head->active_users[clientfd]=1;  //adding user to room table
          printf("User joined room %s\t", roomname);
          res=0; //0 if exists
          // break;  
      }
      else
      {
        printf("roomname diff is %d\n", strcmp(head->name, roomname));
        continue;
      }
      
    }
    
  }
  cnt=0;

  user temp;
  
  char nick_user[NICK_LEN];
  char old_room[NICK_LEN];
  getnick(clientfd,nicknames, nick_user);
  for ( temp = (user)nicknames; temp != NULL; temp=temp->next)
  {
    if (cnt<1)
    {
      cnt++;
      continue;
    }
    else
    { 
      printf("diff nick is %d\n", strncmp(temp->nick , nick_user, strlen(nick_user)-2));
      if (!strncmp(temp->nick , nick_user, strlen(nick_user)-2) && res==0)
      {
        strncpy(old_room, temp->current_salon, NICK_LEN);
        quitRoom(rooms, clientfd, old_room);
        strcpy(temp->current_salon, roomname);  
        memset(roomname, '\0', NICK_LEN);    
        sprintf(roomname, "\nYou joined %s and quitted %s\n", temp->current_salon, old_room);
        printf("quitting room %s successfully\n", old_room);
        printf("Message to send is %s\n", roomname);
        return roomname;
      }
      else
      {
      continue;
      printf("nick diff is %d\n", strncmp(temp->nick , nick_user, strlen(nick_user)-2));
      }
      
    }
    
  }

  free(head);
  free(temp);
    return "__FAIL__";

}

int exist_user(char * nick, user* nicknames){ 
  user head;
  int cnt=0;
  for ( head = (user)nicknames; head != NULL; head=head->next)
  {
    if (cnt<1)
    {
      cnt++;
      continue;
    }
    else
    {

      if (!strcmp(head->nick, nick))
      {
        printf("user found %s\n", head->nick);
        
        return head->fd;
      }
      continue;
    }
    
  }
    head=NULL;
    free(head);
  return -1;
  
  

}
int exist_room(char * name, room_ptr* rooms){ 
  //returns 0 if room exists 1 else
  room_ptr head;
  char name_trunc[NICK_LEN];
  strncpy(name_trunc, name, strlen(name)-1);
  int cnt=0;
  for ( head = (room_ptr)rooms; head != NULL; head=head->next)
  {
    if (cnt<1)
    {
      cnt++;
      continue;
    }
    else
    {
      if (!strcmp(head->name, name_trunc))
      {
        printf("room found %s\n", head->name);
        
        return 0; //0 if exists
      }
      continue;
    }
    
  }
    head=NULL;
    free(head);
  // printf("user not in nicknames %s\n", nick);
  return 1;
  
  

}
char* getnick(int fd, user* nicknames, char* dest){ 
  user head;
  int cnt=0;
  for ( head = (user)nicknames; head != NULL; head=head->next)
  {
    if (cnt<2)
    {
      cnt++;
      continue;
    }
    else
    {
      if (head->fd==fd)
      {
        printf("user found %d\n", head->fd);
        strncpy(dest, head->nick, strlen(head->nick));
        return dest;
      }
      continue;
    }
    
  }
    head=NULL;
    free(head);
  printf("user not in nicknames\n");
  strcpy(dest, "User does not exist");
  return dest;
  
  

}


// message types processing 

void proc_p2p(struct message *msg, int type, int clientfd){
  struct message * msg2send=(struct message *)malloc(sizeof(struct message));//allocating message to send 
  char receiver_name[NICK_LEN], sender[NICK_LEN], filename[NICK_LEN];
  strncpy(receiver_name, msg->nick_sender, NICK_LEN);
  getnick(clientfd,&nicknames, sender );  
  strncpy(filename, msg->infos, strlen(msg->infos));
  /* dbg */
  printf("pld is %d\t", msg->pld_len);
  printf("nick to send to is %s\t", receiver_name);
  printf("file is %s\t", msg->infos);
  printf("sender is %s\t", sender);
  if (msg->pld_len==0)
  { //clientside error$
    printf("client side file error user %d \n", clientfd);
  }
  else
  {
  
    int receiver_fd=exist_user(receiver_name,&nicknames);

  
  if (type==11)
  {
     //file_request
    if (receiver_fd==-1)
    {//receiver unknown
      strcpy(msg2send->infos, "User not found");
      msg2send->type=3;//echo
      msg2send->pld_len=strlen(msg->infos);
      if(echo2client(*msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
       }
    }
    else
    {//user exists
    //send request to user
    memset(msg2send->infos, '\0', NICK_LEN);
    strncat(msg2send->infos, sender, strlen(sender));
    strcat(msg2send->infos, " wants to send file: ");
    strncat(msg2send->infos, filename, strlen(filename));
    strcat(msg2send->infos, ", YES/NO \n");
    msg2send->pld_len=strlen(msg2send->infos);
    msg2send->type=3;//echo
    printf("receiver fd is %d\n", receiver_fd);
     if(echo2client(*msg2send,  receiver_fd)==1){ //err
      printf("Echo message not delivred to client : %d", receiver_fd);
            }
          struct message msg_rcvd;
					memset(&msg_rcvd, 0, sizeof(struct message));
          char buf_from_receiver[MSG_LEN];
					memset(buf_from_receiver, '\0', MSG_LEN);
            if (recv(receiver_fd,(void*)&msg_rcvd, sizeof(struct message), 0)>0)
            {
                if (msg_rcvd.type==FILE_ACCEPT)
                {
                    //send file_send to sender
                    node info_node;
                    char buff[NICK_LEN];
                     //file accept sent from receiver deliver response to sender
                      memset(msg2send, 0, sizeof(struct message));
                      strcpy(msg2send->infos, "Transfer accepted\n");
                      msg2send->type=FILE_ACCEPT;
                      strcpy(msg2send->nick_sender, sender);
                      msg2send->pld_len=strlen(msg2send->infos);
                      printf("file accept\n");
                      info_node=adresselist(&client_list,receiver_fd);
                      if(echo2client(*msg2send,  clientfd)==1){ //err
                      printf("Echo message not delivred to client : %d", receiver_fd);}
                      memset(msg2send, 0, sizeof(struct message));
                      strncpy(msg2send->infos, info_node->addr, strlen(info_node->addr) );
                      strcat(msg2send->infos, ":");
                      sprintf(buff, "%hu\n", info_node->port);
                      strcat(msg2send->infos, buff);
                      msg2send->type=FILE_ACCEPT;
                      strcpy(msg2send->nick_sender, sender);
                      msg2send->pld_len=strlen(msg2send->infos);
                      printf("file accept\n");
                      if(echo2client(*msg2send,  clientfd)==1){ //err
                      printf("Echo message not delivred to client : %d", receiver_fd);
                        }

                }
                else if(msg_rcvd.type==FILE_REJECT){
                    //send file reject to sender
                      //file reject sent from receiver deliver response to sender
                      memset(msg2send, 0, sizeof(struct message));
                      strcpy(msg2send->infos, "Transfer aborted\n");
                      msg2send->type=FILE_REJECT;
                      strcpy(msg2send->nick_sender, sender);
                      msg2send->pld_len=strlen(msg2send->infos);
                      printf("file refused\n");
                      if(echo2client(*msg2send,  clientfd)==1){ //err
                      printf("Echo message not delivred to client : %d", receiver_fd);
                        }
                      }
                
            }
            else{
                      //error
                      memset(msg2send, 0, sizeof(struct message));
                      strcpy(msg2send->infos, "error receiving YES/NO\n");
                      msg2send->type=ECHO_SEND;
                      strcpy(msg2send->nick_sender, sender);
                      msg2send->pld_len=strlen(msg2send->infos);
                      printf("file accept\n");
                      if(echo2client(*msg2send,  clientfd)==1){ //err
                      printf("Echo message not delivred to client : %d", clientfd);
                        }
            }//else
            
        }
    
    }
 
  
  }

}

void proc_nick(struct message * msg, int type, int clientfd){
  
  char nick[NICK_LEN];
  char payload[INFOS_LEN];
  memset(payload, '\0', NICK_LEN);
  strncpy(nick, msg->nick_sender, NICK_LEN);
  strncpy(payload, msg->infos, strlen(msg->infos)  -1); 
  struct message msg2send;
  memset(&msg2send, '\0', sizeof(struct message));
  
  printf(" pld is %i \n", msg->pld_len);
  printf(" type is %s \n",  msg_type_str[msg->type]);
  printf(" nicksender is %s \n", nick);
  printf(" infos is %s", payload);
  ////type 0
 

    
    //send welcome message with nickname 
    char wlcmsg[MSG_LEN]="Welcome to the chat, ";
    
    strcat(wlcmsg, payload);
    strcpy(msg2send.infos, wlcmsg);
    strcpy(msg2send.nick_sender,"");
    msg2send.pld_len=strlen(payload);
    msg2send.type=3;//echo
    printf(" res %d \n", exist_user(msg->infos, &nicknames));

    if(exist_user(payload, &nicknames)!=clientfd && exist_user(payload, &nicknames)!=-1 ){
      msg2send.type=3;//echo
      strcpy(msg2send.infos, "Nickname taken, enter a new nickname\n");
      msg2send.pld_len=strlen(msg2send.infos);
      strcpy(msg2send.nick_sender,"EIRBCHAT");
      echo2client(msg2send,clientfd);
    }
    else {
        strncpy(payload, msg->infos, strlen(msg->infos)  -1); 

      if (exist_user(payload,&nicknames)==-1 )
    {
      //nickname not in the list, add user
      char* payload1;
      if(NULL==(payload1=strtok(payload, " "))){
        nicknames=addNick(&nicknames, clientfd, payload1);

      }
      else
      {
      nicknames=addNick(&nicknames, clientfd, payload);
      }
      
      afficheUser(&nicknames);
      countUser(&nicknames);
     if (send(clientfd, &msg2send, sizeof(msg2send), 0) <0)
    {
      perror("Failed to deliver nic_welcome_message to user");
    }
    memset(&msg2send, 0, sizeof(struct message));
    //flag=0;
    }
    
   
    
    }
}


void proc_who(struct message * msg, int type, int clientfd){
  //filling fields for received msg
  char nick[NICK_LEN];
  char payload[NICK_LEN];
  strcpy(nick, msg->nick_sender);
  memset(payload, '\0', INFOS_LEN);
  strcpy(payload, msg->infos);
  struct message *msg2send_ptr=(struct message *)malloc(sizeof(struct message));

  //debug
  printf(" pld is %i \n", msg->pld_len);
  printf(" type is %s \n",  msg_type_str[msg->type]);
  printf(" nicksender is %s \n", nick); //think about changing nick from "" to real nick correspondance with fd
  printf(" infos is %s \n", payload);
//filling for msg to send
  strcpy(msg2send_ptr->nick_sender, "EIRBCHAT");
  memset(msg2send_ptr->infos, '\0', INFOS_LEN);

 int pld_len;
  if(type==1){ //NICKLIST
  
    //filling fields for msg to send
    if (countUser(&nicknames)<=2) //server and null 
     {strcpy(msg2send_ptr->infos, "No online users ftm \n");
      msg2send_ptr->pld_len=strlen(msg2send_ptr->infos);
      if(echo2client(*msg2send_ptr, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
     }
     
    else
    
    { //prevoir de grands messages
    //si il y a des users
      char buff[MSG_LEN];
      memset(buff, '\0', MSG_LEN);
      puts_afficheruser(&nicknames, buff);
      pld_len=strlen(buff);
      printf("payload length is %d", pld_len);
      
      msg2send_ptr->type=3;
      if (pld_len<=INFOS_LEN)
      {
        strncpy(msg2send_ptr->infos, buff, INFOS_LEN);
        msg2send_ptr->pld_len=pld_len;
         if(echo2client(*msg2send_ptr, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
          }
      }
      else{

      while (pld_len>0)
      { //remplir le buffer des donnees a envoyer
          char temp[INFOS_LEN];
          strncpy(temp, buff, INFOS_LEN-1);
          
          strncpy(msg2send_ptr->infos, temp, INFOS_LEN );
          echo2client(*msg2send_ptr, clientfd);
          printf("big who msg to send is %s\n", msg2send_ptr->infos);  
          pld_len-=INFOS_LEN;
          memset(msg2send_ptr->infos, '\0', INFOS_LEN);

      }
      }
    }  
    
    
    
  //clean
  // memset(msg2send_ptr->infos, '\0', INFOS_LEN); 
  }


  else if (type==2) //whois
  {
    //    has weird behaviour when calling who

    //send requested user informations
    memset(payload, '\0', NICK_LEN);
    strncpy(payload, msg->infos, strlen(msg->infos)-1);
    printf("result is %d\n", exist_user(msg->infos, &nicknames));
    if (exist_user(payload,&nicknames)==-1)
    { 
     memset(payload, '\0', sizeof(payload));

     strncpy(payload, "Nonexisting user ", 18);
     strcat(payload, msg->infos);
     //sprintf(payload, "Non existing user %s\n", msg->infos);
     msg2send_ptr->type=3;//echo
     strcpy(msg2send_ptr->nick_sender,"EIRBCHAT");
     msg2send_ptr->pld_len=strlen(payload);
     strcpy(msg2send_ptr->infos, payload);
     echo2client(*msg2send_ptr, clientfd);

    }
    else
    {//user exists

      memset(payload, '\0', NICK_LEN);
      strncpy(payload, msg->infos, strlen(msg->infos)-1);
      node info_node;

      char usernickconsts[INFOS_LEN]="user conn since "; //initializing const to send
      int userfd=exist_user(payload, &nicknames);//getting the userfd from his nick
      
      //adresselist(&client_list, userfd)   ;


      info_node=adresselist(&client_list, userfd );
      if (info_node==NULL)
      {

          
      }
      
      printf("time conn is %s\n",info_node->time_connection);
      strcat(usernickconsts, info_node->time_connection);
      char ipport[NICK_LEN];
      sprintf(ipport," [%s:%d] \n", info_node->addr,info_node->port);
      strcat(usernickconsts, ipport);
      printf("debg %s\n",usernickconsts);
      memset(&wilcard, 0, sizeof(struct message)); //flush!
      memset(wilcard.infos,'\0',NICK_LEN);
      strcpy(wilcard.infos, usernickconsts);
      wilcard.type=3;
      wilcard.pld_len=264;
      strcpy(wilcard.nick_sender,"EIRBCHAT");
      echo2client(wilcard, clientfd);
      
    
    } 

    //}
    
    //free((void *) &wilcard); is freed at the end of main
     




  }
  
}
void proc_multicast(struct message * msg, int type, int clientfd){
  char nick[NICK_LEN];

  char payload[INFOS_LEN];
  memset(payload, '\0', NICK_LEN);
  strncpy(nick, msg->nick_sender, NICK_LEN);
  strncpy(payload, msg->infos, strlen(msg->infos) -1); 
  struct message msg2send;
  memset(&msg2send, '\0', sizeof(struct message));
  
  printf(" pld is %i \n", msg->pld_len);
  printf(" type is %s \n",  msg_type_str[msg->type]);
  printf(" nicksender is %s \n", nick);
  printf(" infos is %s\n", payload);

  if (type==6)
  {//create
  char roommsg[MSG_LEN]="You created a new room:  ";
    
    strcat(roommsg, payload);
    strcpy(msg2send.infos, roommsg);
    strcpy(msg2send.nick_sender,"");
    msg2send.pld_len=strlen(payload);
    msg2send.type=3;//echo 
    if (exist_room(msg->infos,&rooms))
    {//existroom returns 0 if success
      //room doesnt exist
      rooms=addRoom(&rooms, clientfd,payload );
      char *joined_msg;
      joined_msg=joinRoom(&rooms, &nicknames, payload, clientfd ); //adding user to his newly created room
      afficheRoom(&rooms);
      if (*joined_msg=='_')
      {
      if (send(clientfd, &msg2send, sizeof(msg2send), 0) <0)
      {
      perror("Failed to deliver nic_welcome_message to user");
      }
      }
      else
      {
      strncat(msg2send.infos, joined_msg, strlen(joined_msg));
      countRoom(&rooms);
      if (send(clientfd, &msg2send, sizeof(msg2send), 0) <0)
      {
      perror("Failed to deliver nic_welcome_message to user");
      }
      memset(&msg2send, 0, sizeof(struct message));      }
      
      
      
    }
    else
    {
        //room exists
        memset(msg2send.infos, '\0', INFOS_LEN);
        memset(roommsg, '\0', MSG_LEN);
        strcpy(msg2send.infos, "Room exists. Enter a new name");
        msg2send.pld_len=strlen(msg2send.infos);
        msg2send.type=3;
        if (send(clientfd, &msg2send, sizeof(msg2send), 0) <0)
          {
            perror("Failed to deliver nic_welcome_message to user");
          }
        memset(&msg2send, 0, sizeof(struct message));

    }
    
    
    
  }
    if (type==7)
  {
      // MULTICAST_LIST
      memset(&msg2send, 0, sizeof(struct message));
    //filling fields for msg to send
      msg2send.type=3;//echo
    if (countUser(&nicknames)<=2) //server and null 
     {strcpy(msg2send.infos, "No online users ftm \n");
      msg2send.pld_len=strlen(msg2send.infos);
      if(echo2client(msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
     }
     else
     {
       //existing users
       char payload[INFOS_LEN];
       puts_afficherrooms(&rooms, payload, clientfd);
       msg2send.pld_len=strlen(payload);
       strncpy(msg2send.infos,payload, INFOS_LEN);
        if(echo2client(msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
     }
     
  }

  if (type==8)
  {

    //join a channel if room exists
    
    if (!exist_room(payload, &rooms))
    {//if room exists
    //adding user to the room and sending wlcmsg
        char wlcmsg[INFOS_LEN]="You have joined room: ";
        strcat(wlcmsg, payload);
        msg2send.type=3;
        printf("room to join is %s", payload);
        strncpy(payload, msg->infos, strlen(msg->infos)-1);
        if(!strcmp(joinRoom(&rooms,&nicknames, payload, clientfd),"__FAIL__")){
          printf("failed to join selected room\n");
        }
        strncpy(msg2send.infos,wlcmsg, strlen(wlcmsg));
        strncpy(msg2send.nick_sender, "EIRBCHAT",9);
        if(echo2client(msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
    }
    else
    {
      //room doesnt exist 
      memset(&msg2send, 0, sizeof(struct message));
      strncpy(msg2send.infos, "Room not found, /channel_list to see rooms\n",INFOS_LEN);
      msg2send.type=3;
      msg2send.pld_len=strlen(msg2send.infos);
      strcpy(msg2send.nick_sender,"EIRBCHAT");
       if(echo2client(msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
    }

    }

    if (type==10)
    {
      //see if room exists
  
      if (!exist_room(msg->infos, &rooms))
      {
        //disconnect user from room
      
        strncpy(payload, msg->infos, strlen(msg->infos)-1);
        quitRoom(&rooms,clientfd, payload); //quitting room
        joinRoom(&rooms, &nicknames, msg->infos, clientfd); //getting him back to mainroom

      //see if theres still someone in room kill room
        msg2send.type=3;//echo
        sprintf(msg2send.infos, "You quitted your current room\n");

       if(echo2client(msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
      }
  
    else
    {
      //room doesnt exist 
      memset(&msg2send, 0, sizeof(struct message));
      strncpy(msg2send.infos, "Room not found, /channel_list to see rooms\n",INFOS_LEN);
      msg2send.type=3;
      msg2send.pld_len=strlen(msg2send.infos);
      strcpy(msg2send.nick_sender,"EIRBCHAT");
       if(echo2client(msg2send, clientfd )==1){ //err
      printf("Echo message not delivred to client : %d", clientfd);
    }
    }//endelse
    }//quit salon
    
  }//end_func

  
  


void proc_msg(struct message * msg, int type, int clientfd){
  struct message msg2send;
  char nick[NICK_LEN];
  memset(&msg2send, 0, sizeof(struct message));
  memset(&msg2send.infos, '\0', INFOS_LEN);
  //debug
  printf(" pld is %i \n", msg->pld_len);
  printf(" type is %s \n",  msg_type_str[msg->type]);
  printf(" nicksender is %s \n", msg->nick_sender); //think about changing nick from "" to real nick correspondance with fd
  printf(" infos is %s \n", msg->infos);

  if (type==5)
  { //MSGALL
      if(countUser(&nicknames)<=2)
      {
        //no users yet
        msg2send.type=3;//echo
        
        strcpy(msg2send.infos, "No users yet in the chat");
         if (echo2client(msg2send, clientfd)==1)
          {
            printf("Error delivering brdcst msg\n");
          }
      }
      else
      { //users in the chat
        printf("user %s has fd %d \n",getnick(clientfd, &nicknames, nick), clientfd );
        strcat(msg2send.infos, "[");

        strncat(msg2send.infos, nick, strlen(nick));
        strcat(msg2send.infos, "] ---> ");

        strcat(msg2send.infos, msg->infos);
        msg2send.pld_len=strlen(nick);
        strcpy(msg2send.nick_sender, nick);
        printf("msgall payload is %s", msg2send.infos);
        //send msg by nick to all
        int num_online;
        num_online=count(&client_list)-1;
        node temp_head=(node)&client_list;
        temp_head=temp_head->next;
        for (int i = 0; i < num_online; i++)
        {
          if (echo2client(msg2send, temp_head->fd)==1)
          {
            printf("Error delivering brdcst msg\n");
          }
            temp_head=temp_head->next; //advance

       }
            memset(nick, '\0', NICK_LEN);
  }
}


if (type==4)
{ //unicast msg
  //get target nick and infos 
  //nicksender is the target clientfd gives sender nick

  if (exist_user(msg->nick_sender,&nicknames)==-1)
  {
    strcpy(msg2send.infos,"User does not exist, stop using bandwith");
    strcpy(msg2send.nick_sender,"EIRBCHAT");
    msg2send.pld_len=strlen(msg2send.infos);
    if (echo2client(msg2send,clientfd)==1)
      {
        perror("failed to deliver");
      }
  }
  else
  {
  //user exists
  getnick(clientfd, &nicknames, nick);
  strcat(msg2send.infos, "[");

  strncat(msg2send.infos, nick, strlen(nick)); // [toto] ---> msg
  strcat(msg2send.infos, "] ---> ");
  strcat(msg2send.infos, msg->infos);
  strcpy(msg2send.nick_sender, nick);
  msg2send.pld_len=strlen(msg2send.infos);
 int target_fd=exist_user(msg->nick_sender,&nicknames);
if (echo2client(msg2send,target_fd)==1)
      {
        perror("failed to deliver");
      }
  }
}
}

///////////////////////////////////////////e////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////



void main_thread(int sockfd) {
		struct message msg2recv;
    int type;
	while (1) {
		// Cleaning memory
		memset(&msg2recv, 0, sizeof(struct message));
		// Receiving struct
		if (recv(sockfd, &msg2recv, sizeof(struct message), 0) <= 0) {
			break;
		}
      char payload[INFOS_LEN];
      strcpy(payload, msg2recv.infos);
      type=msg2recv.type;
      char buff[MSG_LEN]="ECHO:\t";
      
      //switch on type
      switch (type)
      {
      case 0:
        proc_nick(&msg2recv, type, sockfd);
        break;
         case 1:
        proc_who(&msg2recv, type, sockfd);

        break;
        case 2: 
        proc_who(&msg2recv, type, sockfd);
        break;
        case 3:
        // Sending message (ECHO) in case of client writing gibberish
            if (!strcmp(payload, "/quit\n")) //dumbo is quitting
            {
              if (send(sockfd, (const void *) " *PLAYS FAREWELL BALLAD, sniff sniff...* ", strlen(buff), 0) <= 0) {

		        	perror("INTERNAL ERROR ECHO");
		          }

            }
            else
            {
            
            
            strcat(buff, payload);
		        if (send(sockfd, (const void *)buff, strlen(buff), 0) <= 0) {

		        	perror("INTERNAL ERROR ECHO");
		        }
		        printf("echoed : [%s] to client %d who wrote gibberish \n", buff,  sockfd);
            }
      case 5:
        proc_msg(&msg2recv, type, sockfd);
      break;
      case 6:
        proc_multicast(&msg2recv, type, sockfd);
      break;
      case 7:
        proc_multicast(&msg2recv, type, sockfd);          
      break;
      case 4:
        proc_msg(&msg2recv, type, sockfd);
        break;
      case 8:
        proc_multicast(&msg2recv, type, sockfd);
      break;
      case 10:
        proc_multicast(&msg2recv, type, sockfd);
      break;
      case 11:
        proc_p2p(&msg2recv, type, sockfd);
      break;
     

      default:
          
            
        break;
      }
		
    
		
	}
 
  /*
  void * ret;
  ret=&sockfd;
  pthread_exit(ret);
*/
}



int handle_bind(const char * port) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
		rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}
