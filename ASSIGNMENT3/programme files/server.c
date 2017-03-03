#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
void error(char *msg);
void validation(int argc);
void *handler(void *);
void *handler_read(void *);
void *handler_write(void *);
#define MAX_CLIENT 20
int main(int argc, char *argv[])
{
	int old_socket,new_socket,port,bind_flag,l,n;
	int *client_sock;
	struct sockaddr_in server_addr,client_addr;
	validation(argc);
	// Create IPv4 Internet protocol + Stream socket
	if((old_socket = socket(AF_INET,SOCK_STREAM,0))<0){
		error("can not create 1st socket");
	}
	// Bind the socket with Port no
	bzero((char *) &server_addr,sizeof(server_addr));
	port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if((bind_flag = bind(old_socket,(struct sockaddr *)&server_addr,
		sizeof(server_addr))) < 0){

		error("Binding error");
	}
	
	// Listen to the port no
	listen(old_socket,5);
	// accept the client socket 
	printf("%s\n","Server is waiting for incoming connections .........." );
	l = sizeof(client_addr);
	//new_socket = accept(old_socket, (struct sockaddr *)&client_addr,&l);
	while((new_socket = accept(old_socket, (struct sockaddr *)&client_addr,&l))){
		pthread_t con_thread;
		client_sock = (int *)malloc(1);
		*client_sock = new_socket;
		if(pthread_create((pthread_t *)&con_thread,NULL,handler,(void *)client_sock) < 0){
			printf("%s\n","Can not create thread , Drop connection" );
		}
		else{
			printf("Thread has been created and task has been performed");
			printf("%d.%d.%d.%d\n",
  						(int)(client_addr.sin_addr.s_addr&0xFF),
  						(int)((client_addr.sin_addr.s_addr&0xFF00)>>8),
  						(int)((client_addr.sin_addr.s_addr&0xFF0000)>>16),
  						(int)((client_addr.sin_addr.s_addr&0xFF000000)>>24));
		}
	}
	if(new_socket < 0){
			error("Error creating new socket final socket");
	}
	return 0;
}
void error(char *msg){
	perror(msg);
	exit(1);
}
void validation(int argc){
	if (argc<2){
		error("less argument");
	}
}
void *handler(void * sock){
	char data[256];
	int n;
	pthread_t read_thread,write_thread;
	int client_sock = *(int*)sock;
	int *temp_sock = (int *)malloc(1);
	*temp_sock = client_sock;
	bzero(data,256);
	/*while(1){
		if((n = read(client_sock,data,255)) < 0){
			error("Error reading client socket");
		}
		else{
			if(data[0] == '\0'){
				break;
			}
			printf(" The received messege %s\n", data);
			bzero(data,256);
		}
		printf("%s\n","ack" );
	}*/	
	if(pthread_create((pthread_t *)&read_thread,NULL,handler_read,(void *)temp_sock) < 0){
			printf("%s\n","Can not create thread for reading , Drop connection" );
	}
	if(pthread_create((pthread_t *)&write_thread,NULL,handler_write,(void *)temp_sock) < 0){
			printf("%s\n","Can not create thread for writing , Drop connection" );
	}
	else{
		printf("both read and write thread has been created");
	}
}
void *handler_read(void *sock){
	int client_sock = *(int*)sock;
	int n;
	char data[256];
	while(1){
		if((n = read(client_sock,data,255)) < 0){
			error("Error reading client socket");
		}
		else{
			if(data[0] == '\0'){
				break;
			}
			printf(" The received messege %s\n", data);
			bzero(data,256);
		}
	}
}
void *handler_write(void *sock){
	int client_sock = *(int*)sock;
	int n;
	// Retrieve messege from database and populate char array and the databse should contain the socket id as primary key 
	char data[]="messege from server from write thread"; 	// This will be repalced by messege retrieved from database for two connected users
	while(1){
		sleep(5);
		if((n = write(client_sock,data,255)) < 0){
			error("Error writing client socket");
		}
		else{
			printf("Has been written to socket no %d\n",client_sock);
		}
		//write(client_sock,"hi",3);
	}
	//printf("%s\n","write socket" );
}		