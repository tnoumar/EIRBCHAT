

  ```

 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ 
▐░▌               ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     
▐░█▄▄▄▄▄▄▄▄▄      ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     
▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     
▐░█▀▀▀▀▀▀▀▀▀      ▐░▌     ▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌     ▐░▌     
▐░▌               ▐░▌     ▐░▌     ▐░▌  ▐░▌       ▐░▌▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     
▐░█▄▄▄▄▄▄▄▄▄  ▄▄▄▄█░█▄▄▄▄ ▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀         ▀       ▀      
                                                                                                        
 
 ```                                                      



**EIRBCHAT**

EIRBCHAT is a simple IRC messaging service fully written in C that was implemented in the context of an academic Network Programming project (RE216).

# Context

This project consists of the realization of a great classic of network programming, a practical case of instant client / server discussion. As an example and as a curiosity, you can take a look at the IRC (Internet Relay Chat) protocol originally defined by RFC1459.

The objective of the project is to create a client / server chat application in C allowing messages to be exchanged between 2 users, between several users, or to all users connected to the network, as well as sending files to each other.

The underlying objective of this project is the manipulation of the network primitives and POSIX C sockets that you have seen in progress (socket (), bind (), listen (), connect (), accept (), send ( ), recv ()), as well as the implementation of communications over TCP / IP.

# Compilation 
In order to compile the project,
 ```                                                      
$ cd src/
$ make 
 ```                                                      

# Server Side 
  ```                                                      
$ cd bin/
$ ./server_exec port
_______ _ ______  ______  _______ _     _ _______ _______ 
(_______) (_____ \(____  \(_______|_)   (_|_______|_______)
 _____  | |_____) )____)  )_       _______ _______    _    
|  ___) | |  __  /|  __  (| |     |  ___  |  ___  |  | |   
| |_____| | |  \ \| |__)  ) |_____| |   | | |   | |  | |   
|_______)_|_|   |_|______/ \______)_|   |_|_|   |_|  |_|   
                                                           BINDIN' ....start accepting...
nouveau client connecté[127.0.0.1:34666]
 fd : 0 	port : 0 	 addr :(null) 	conn_time : 	
 fd : 0 	port : 0 	 addr :root 	conn_time :2021/06/19@13:29 	
 fd : 4 	port : 34666 	 addr :127.0.0.1 	conn_time :2021/06/19@13:30 	

 ```                                                      
The server keeps track of connected clients and shows some logging on the terminalis

# Client Side 

 ```                                                      
$ cd bin/
$ ./client_exec server_ip_address server_port
SERVER -> : 	[EIRBCHAT]  : please login with /nick <your pseudo>
/nick tnoumar

 [CLIENT]:  	SERVER -> : 	Welcome to the chat, tnoumar
 ```                                                      

Once you execute the previous commands, you get this terminal interface which resembles an IRC UI.

# List of available commands
  - ``` /nick username```: Choose a cool username.
  - ``` /who```: Show the list of connected users (in the current chat room).
  - ``` /whois username```: Show information about a specific username.
  - ``` /msgall msg```: Send a msg to everyone in the chat room.
  - ``` /msg user1 msg```: Send a msg to user1 in the chat room.
  - ``` /create channel```: Create a channel and join it.
  - ``` /join channel```: Join channel.
  - ``` /quit channel```: Quit channel.
  - ``` /send user1 file.txt```: Send a file to a specific user.





This project was tested locally. It should be working fine on a remote server with proper configuration.

