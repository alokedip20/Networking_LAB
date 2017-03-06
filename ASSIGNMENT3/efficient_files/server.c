#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <limits.h>
#include <arpa/inet.h>
using namespace std;			// mysqlite.h uses <std::string> 
#include "mysqlite.h"
void error(string msg);
void validation(int argc);
bool contain_ter(char data[]);
void parse_message(char data[],int client_sock);
void message(string *parse,char data[]);
string sanitization(string data);
string user_refine(string data);
void *handler(void *);
void *handler_read(void *);
void *handler_write(void *);
#define MAX_CLIENT 20
#define DATABASE_NAME "Networking_Assignmenet3.db"
#define TABLE_NAME "USERS"
unsigned long int TIME_LIMIT = ULLONG_MAX;
int main(int argc, char *argv[])
{
	int old_socket,new_socket,port,bind_flag,l,n;
	int *client_sock;
	struct sockaddr_in server_addr,client_addr;
	validation(argc);                     // Validate proper command line arguments

	cout<<"TIME LIMIT : "<<TIME_LIMIT<<endl; 
	
	/* Create IPv4 Internet protocol + Stream socket
		** AF_INET : for domain -> IPv4
		** SOCK_STREAM : type of socket -> stream Socket (TCP) for ensuring reliable full duplex communication
		** 0 : protocol famility : single protocol -> 0
	*/
	if((old_socket = socket(AF_INET,SOCK_STREAM,0))<0){
		error("can not create 1st socket");
	}
	bzero((char *) &server_addr,sizeof(server_addr));
	port = atoi(argv[1]);
	/*
		For binding the sockt with particular address we have to assign those values in server_addr
		** sin_family : address family -> our case it is AF_INET
		** sin_port : port address 2 byte data -> command line argument and >1024 : port below 1024 is called privilaged port and can not be aasigned manually
		** sin_addr : internet address which will be bind with the socket. To bind any ip address we will use 
					  INADDR_ANY (0.0.0.0). Here sin_addr is of type in_addr and in_addr has only one element which stores the internet address 4 byte data and that is s_addr				  
	*/
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);     // htons convert the 2 byte integer to network byte order
	//server_addr.sin_addr.s_addr = INADDR_ANY;//inet_addr("10.2.88.231");
	server_addr.sin_addr.s_addr = inet_addr("10.2.88.231");
	if((bind_flag = bind(old_socket,(struct sockaddr *)&server_addr,sizeof(server_addr))) < 0){
		error("Binding error");
	}
	
	/* Listen to the port no
		** socket will be used to accept incoming connection by accept() system call
		** 1st argument : socket_descriptor returned by socket() system call
		** 2nd argument : backlog : maximum no of pending connections for the socket,which may be allowed to grow
	*/
	printf("%s\n",inet_ntoa(server_addr.sin_addr) );
	listen(old_socket,5);
	

	cout <<"Server is waiting for incoming connections .........."<<endl;
	l = sizeof(client_addr);

	/*
		** Database has been created
		** Table having 4 attributes 
				*** userID -> socketfd (primary key)
				*** Username
				*** Data -> most recent message for the user will be stored
				*** Status -> Online / Offline 
	*/
	create_database(DATABASE_NAME);
	create_table(TABLE_NAME);

	/* accept the client socket 
		** 1st argument : socket_descriptor that has been returned by socket()
		** 2nd argument : pointer to the structure "sockaddr" which contains the address of the client
		** 3rd argument : length of client_addr which is of type sockaddr_in

		accept() will take the 1st pending connection (listen ()) and create a new socket by which any future communication will take place
	*/
	
	while((new_socket = accept(old_socket, (struct sockaddr *)&client_addr,(socklen_t*)&l))){
		pthread_t con_thread;
		client_sock = new int[1];
		*client_sock = new_socket;
		/*
			** For each client one thread will be created and the corresponding socketfd will be passed to the thread_handler function
			** 1st argument : pthread_t element -> threadID
			** 2nd argument : attribute list -> here it is NULL
			** 3rd argument : pointer to the function in which task for the thread has been defined
			** 4th argument : pointer to the argument for the handler function -> here only socketfd 
		*/
		if(pthread_create((pthread_t *)&con_thread,NULL,handler,(void *)client_sock) < 0){
			cout<< "Can not create thread , Drop connection"<<endl;
		}
		else{
			cout<<"Thread has been created and task has been performed"<<endl;
		}
	}
	if(new_socket < 0){
			error("Error creating new socket final socket");
	}
	return 0;
}
void error(string msg){
	perror(msg.c_str());
	exit(1);
}
void validation(int argc){
	if (argc<2){
		error("less argument");
	}
}
void *handler(void * sock){
	//char data[256];
	int n;
	/*
		** For each client_thread there will be two dedicated threads. One is for reading from the client socket and update the database
		   and other is for writing data (retrieved from database) into the client socket. 
		** read_thread -> handler_read()
		** write_thread -> handler_write()		
	*/
	pthread_t read_thread,write_thread;
	int client_sock = *(int*)sock;
	int *temp_sock = new int[1];
	*temp_sock = client_sock;
	//bzero(data,256);
	if(pthread_create((pthread_t *)&read_thread,NULL,handler_read,(void *)temp_sock) < 0){
		cout<<"Can not create thread for reading , Drop connection"<<endl;
	}
	if(pthread_create((pthread_t *)&write_thread,NULL,handler_write,(void *)temp_sock) < 0){
		cout<< "Can not create thread for writing , Drop connection"<<endl;
	}
	else{
		cout<<"both read and write thread has been created"<<endl;
	}
}
void *handler_read(void *sock){
	int client_sock = *(int*)sock;
	int n;
	char data[256];
	string parse[2];
	while(1){
		if((n = read(client_sock,data,255)) < 0){
			error("Error reading client socket");
		}
		else{
			if(data[0] == '\0'){
				cout<<"NULLLL READ"<<endl;             //thread termination condition
				break;
			}
			/*
				** Assign user with corresponding socket_descriptor in database
				** contain_ter() will return true if data[] contains '@' symbol
				** insert(user_name,data,status,socket_descriptor)
			*/
			if(!contain_ter(data)){
				char d[255];
				strncpy(d,data,strlen(data)-1);
				if(!strncmp(data,d,strlen(data)-1)){
				}
				insert(d,"",1,client_sock);
			}
			else{
				/*
					** message() will segment destination_client username and corresponding message 
					   and store them into parse[0] and parse[1]
				*/
				message(parse,data);
				parse[1] = sanitization(parse[1]);
				parse[0] = user_refine(parse[0]);
				sleep(1);					// To avoid "database lock" error 
				/*
					** update(user_name,data) ->update the database
				*/							
				update(parse[0],parse[1]);
			}
			bzero(data,256);		// clean the buffer
		}
	}
}
void *handler_write(void *sock){
	int client_sock = *(int*)sock;
	int n;
	unsigned long int K = 0;
	char data[255];
	while(1){
		sleep(1);
		/*
			** parse_message(data[],socketfd) -> retrieve data from the DB corresponding to userID ->socketfd
		*/
		parse_message(data,client_sock);
		if(data[0] == '\0'){
			if(K >= TIME_LIMIT){
				break;			// thread termination condition
			}
			cout<<" TIME lIMIT : "<<TIME_LIMIT<<endl;
			cout<<" +++++++++++++++++++++ VALUE OF K FOR SOCKET "<<client_sock<<" = +++++++++++++++++++ "<<K<<endl<<endl;
			K++;                       
		}
		else{
			K = TIME_LIMIT - 20;
			cout<<" TIME lIMIT : "<<TIME_LIMIT<<" SUB : == "<<K<<endl;
			if((n = write(client_sock,data,255)) < 0){
				error("Error writing client socket");
			}
			else{
				cout<<"Data => "<<data<<" Has been written to socket no "<<client_sock<<endl;
			}
			bzero(data,255);	//clean the buffer
		}
	}
}		
bool contain_ter(char buf[]){
	for (int i = 0; i < strlen(buf); ++i)
	{
		if(buf[i] == '@'){
			return true;
		}
	}
	return false;
}
void message(string *parse,char buf[]){
	string msg="";
	int k = 0;
	for (int i = 0; i < strlen(buf); ++i)
	{
		if(buf[i] != '@'){
			msg += buf[i];
		}
		else{
			parse[k] = msg;
			msg = "";
			k++;
		}
	}
	parse[k] = msg;
}
void parse_message(char data[],int client_sock){
	record R;
	string msg="";
	R = retrieve(client_sock);
	msg = R.message;
	strcpy(data,msg.c_str());
	return;
}
string sanitization(string data){
	return data.substr(0,data.length()-1);
}
string user_refine(string data){
	string msg="";
	for (int i = 0; i < data.length(); ++i)
	{
		if(data[i] != ' '){
			msg += data[i];
		}
	}
	return msg;
}