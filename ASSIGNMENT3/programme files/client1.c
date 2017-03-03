#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
void error(char *msg);
void validation(int argc);
int main(int argc, char *argv[])
{
	int client_socket,port,n;
	struct sockaddr_in server_addr;
	struct hostent *server;
	char data[256];
	validation(argc);
	port = atoi(argv[2]);
	client_socket = socket(AF_INET,SOCK_STREAM,0);
	if(client_socket < 0){
		error("socket can not be created");
	}
	server = gethostbyname(argv[1]);
	if(server == NULL){
		error(" No host has been found");
	}
	bzero((char *)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	strncpy((char *)server->h_addr, 
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);	
	server_addr.sin_port = htons(port);
	if(connect(client_socket,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
			error("Can not connect with the server");
	}
	printf("%s\n","enter messege" );
	bzero(data,sizeof(data));
	while(fgets(data,255,stdin) !=NULL){
		if((n=write(client_socket,data,strlen(data))) < 0){
			error("Can not write into socket");
		}
		else{
			printf("%s\n","Succesfully send data" );
			bzero(data,sizeof(data));
		}
	}
	return 0;
}
void validation(int argc){
	if(argc < 3){
		error("Less argument");
	}
}
void error(char *msg){
	perror(msg);
	exit(1);
}