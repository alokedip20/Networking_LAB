#include <iostream>
#include <stdio.h>
#include <sys/socket.h>		
#include <netinet/in.h>
#include <arpa/inet.h>		
#include <netdb.h>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include "my_tftp.h"
using namespace std;
bool validation(int argc);
int connect_to_server(const char *argv[],struct sockaddr_in &server_addr);
void display_server_details(struct sockaddr_in addr);
bool req_packet(unsigned char opcode,char *filename, char request_buf[],int *request_length);
void GET_FILE(char filename[],struct sockaddr_in server_addr,int socket);
int ack_packet(int block,char ack_buf[]);
/*
	** server_address port <-g filename> <-p filename>
*/
int main(int argc, char const *argv[])
{
	if(!validation(argc)){
		perror(" invalid argument");
		return ERROR;
	}
	int client_socket = -1;
	struct hostent *server = NULL;
	struct sockaddr_in server_addr;
	server = gethostbyname(argv[1]);
	if(server == NULL){
		perror(" Can not get server by hostname. Please try to use IP format");
		return ERROR;
	}
	cout<<" Server has been found successfully"<<endl;
	bzero((char *)&server_addr , sizeof(server_addr)); 	// clean the var
	if((client_socket = connect_to_server(argv,server_addr)) < 0){
		perror(" Can not connect to the server or socket error");
		return ERROR;
	}
	display_server_details(server_addr);
	unsigned char opcode = 0x06;
	char filename[50];
	switch(argv[3][1]){
		case 'g':
			opcode = RRQ;
			strncpy(filename,argv[4],strlen(argv[4]));	
			printf("%s -> %02x --> %s\n","READ REQUEST",opcode,filename );	
			break;
		case 'p':
			opcode = WRQ;
			strncpy(filename,argv[4],strlen(argv[4]));
			printf("%s -> %02x --> %s\n","WRITE REQUEST",opcode,filename );
			break;
		default:
			cout<<" Error Arguments "<<endl;
			return ERROR;
	}
	char request_buf[MAXREQPACKET];
	int request_length;
	/*
		** request packet will be sent to server request can be RRQ or WRQ
	*/
	if(!req_packet(opcode,filename,request_buf,&request_length)){
		perror(" Can not create request packet ");
		return ERROR;
	}
	cout<<" Request Packet has been created "<<endl;
	int request_send_status = -1;
	printf("Opcode : %02x",request_buf[1]);
	if((request_send_status = sendto(client_socket,request_buf,request_length,0,(const struct sockaddr *)&server_addr,
									sizeof(server_addr))) < 0){
		perror(" Can not send request packet ");
		return ERROR;
	}
	cout<<" Successfully sent request packet ==> "<<request_send_status<<endl;
	switch(opcode){
		case 0x01:
			GET_FILE(filename,server_addr,client_socket);
			break;
		case 0x02:
			//POST_FILE();
			break;
		default: 
			cout<<" No predefined function "<<endl;
			return ERROR;
	}
	close(client_socket);
	return 0;
}
int connect_to_server(const char *argv[],struct sockaddr_in &server_addr){
	bzero((char *)&server_addr,sizeof (server_addr));	/*Clear the structure */
  	server_addr.sin_family = AF_INET;	/*address family for TCP and UDP */
  	int port = atoi(argv[2]);
  	server_addr.sin_port = htons (port);
  	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  	int client_socket = -1;
  	client_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  	return client_socket;
}
bool validation(int argc){
	cout<<" Press -g followed by file name to download file and -p followed by file name to upload the file into the tftp server"<<endl;
	return (argc == 5);
}
void display_server_details(struct sockaddr_in addr){
	printf("%s\n","=================================================================" );
	printf("%s\n","You have successfully connected with the server . Details are as below : " );
	printf(" Server Ip -> %s\n",inet_ntoa(addr.sin_addr));
	printf(" Server Port -> %d\n",ntohs(addr.sin_port));
}
bool req_packet(unsigned char opcode,char *filename,char request_buf[],int *request_length){
	int packet_length = sprintf(request_buf,"%c%c%s%c%s%c",0x00,opcode,filename,0x00,"octet",0x00);
	if(packet_length > 0){
		printf("Request packet length => %d\n",packet_length);
		*request_length = packet_length;
	}
	return (packet_length > 0);
}
void GET_FILE(char filename[],struct sockaddr_in server_addr,int socket){
	printf(" FILENAME - %s\n",filename );
	printf(" Server IP - %s\n",inet_ntoa(server_addr.sin_addr));
	printf("Socket - %d\n GOING TO DOWNLOAD FILE FROM THE SERVER\n",socket );
	printf("%s\n","-------------------------------------------------------" );
	FILE *fp = NULL;
	int data_section = 512;			//tftpd will send 516 = 4 + 512 bytes  
	extern int errno;				// So that this var can be modified in case of any error
	int response = data_section + 4;	// server response shoul be 516
	unsigned char file_buffer[MAX_FILE_BUFFER + 1] = {0};		// ascii >= 0 clean the buffer to prevent garbage writing
	unsigned char response_buf[MAX_FILE_BUFFER + 1] = {0};
	char ack_buf[256] = {0};
	int received_packet,next_packet;
	received_packet = 0;
	next_packet = 0;
	int TID = 0;
	struct sockaddr_in anonymous;
	bzero((char *)&anonymous,sizeof(anonymous));
	int len = sizeof(anonymous);
	char *response_handler = NULL;
	int server_opcode;
	bool first_time_response = true;
	int i;
	bool file_open = true;
	while(response == data_section + 4){
		/*
			** receive each chunk of data
		*/
		bzero((char *)response_buf,MAX_FILE_BUFFER);
		bzero((char *)file_buffer,MAX_FILE_BUFFER);
		bzero(ack_buf,256);
		for (i = 0; i < MAX_RETRY; i++)
		{	
			response = -1;
			errno = EAGAIN;
			for (int j = 0; j < TIME_OUT && response < 0 && errno == EAGAIN; j++)
			{
				response = recvfrom(socket,response_buf,sizeof(response_buf)-1,MSG_DONTWAIT,(struct sockaddr *)&anonymous,
					(socklen_t *)&len);
				usleep(1000);
			}
			if(!TID){
				TID = ntohs(anonymous.sin_port);			// get the port from where the response has come
				server_addr.sin_port = htons(TID);		// Consider that port as the main port
			}
			if(response < 0 && errno != EAGAIN){
				printf("Response --> %d error no --> %d\n",response,errno);
				perror(" Client could not receive any response from EAGAIN error");
			}
			if(response < 0 && errno == EAGAIN){
				printf("Response --> %d error no --> %d\n",response,errno);
				perror(" Client could not receive any response from error.. Time Out");				
			}
			/*
				** Got a valid >0 response
			*/
			else {
				/*
					** Server has responded but need to validate the intended server response
				*/
				if(anonymous.sin_addr.s_addr != server_addr.sin_addr.s_addr){
					printf("Response from Different server -----> %s\n",inet_ntoa(anonymous.sin_addr));
					i--;
					continue;
				}
				/*
					** Server IP address is matched. Intended server
				*/
				else{
					if(TID != htons(server_addr.sin_port)){
						printf(" Different Transmission Identifier Act - %d But - %d \n",TID,anonymous.sin_port );
						int error_length = sprintf((char *)response_buf,"%c%c%c%c%s%c",0x00,ERR,0x00,0x05,"Bad/Unknown TID",0x00);
						if(error_length != sendto(socket,response_buf,error_length,0,(const struct sockaddr *)&server_addr,
							sizeof(server_addr))){
							perror("can not send error message");
						}
						i--;
						continue;
					}
					/*
						** All is Ok. Store it to file 
							*** DATA PACKET 2 + 2 + 512
					*/
					response_handler = (char *)response_buf;
					response_handler++;							// put the pointer to buf[1]
					server_opcode = *response_handler++;		// opcode storage
					received_packet = *response_handler++ << 8;
		      		received_packet &= 0xff00;
		      		received_packet += (*response_handler++ & 0x00ff);	// pointer points to the begining of the actual data section
					memcpy((char *)file_buffer,response_handler,response-4); 	// As first 4 byte has already been processed					
		      		printf(" The opcode = %02x --- The Block received -- %d\n",server_opcode,received_packet);
					/*
						** First time server response handler data limit exceed
					*/
					if(first_time_response){
						if(response > 516){
							data_section = response - 4;
						}
						else if(response < 516){
							data_section = 512;		// to make ensure that this will be last file chunk
						}
						first_time_response = !first_time_response;
					}
					/*
						** To make ensure that the received packet is data packet
					*/
					if(server_opcode != 0x03){
						printf(" The received packet is not data packet. Opcode %02x -- Error Message ==> %s\n",server_opcode,response_handler);
						if(server_opcode > 0x05){
							printf(" Wrong opcode %02x\n",server_opcode );
							int error_length = sprintf((char *)response_buf,"%c%c%c%c%s%c",0x00,ERR,0x00,0x04,"Illegal TFTP operation",0x00);
							if(error_length != sendto(socket,response_buf,error_length,0,(struct sockaddr *)&server_addr,
												sizeof(server_addr))){
								perror(" Wrong bytes error message sent");
							}
						}
					}
					else{
						if(file_open){
							fp = fopen(filename,"w");
							if(fp == NULL){
								perror(" Filecan not be opened for writing");
								exit(0);
							}
							file_open = !file_open;
						}
						next_packet++;
						int ack_length = ack_packet(next_packet,ack_buf);
						if(ack_length != sendto(socket,ack_buf,ack_length,0,(struct sockaddr *)&server_addr,
												sizeof(server_addr))){
							printf("%s\n"," Can not sent Ack Correctly  for next data packet" );
						}
						break;
					}
				}
			}
		}
		if(i == MAX_RETRY){
			printf("%s\n","Exhausted with retrying ........ ");
			if(!file_open){
				fclose(fp);
			}
			return;
		}
		if(fwrite((char *)file_buffer,1,response-4,fp) != (unsigned int)(response-4)){
			fclose(fp);
			sync();
			printf("%s\n","Can not write data to file Sorry ......... :(" );
			return;
		}
	}
	if(response < data_section + 4){
		/*
			** Last chunk data
		*/
		int ack_length = ack_packet(next_packet,ack_buf);
		if(ack_length != sendto(socket,ack_buf,ack_length,0,(struct sockaddr *)&server_addr,
								sizeof(server_addr))){
			printf("%s\n"," Can not sent Ack Correctly  for next data packet" );
		}
		else{
			printf("%s\n","Successfully send last ACK....... :)" );
			fclose(fp);
			sync();
			return;
		}
	}
}
int ack_packet(int block,char ack_buf[]){
	/*
		** 4 Byte data:
			*** 2 byte opcode 0x0004
			*** 2 byte block number 
	*/
	int packet_length = sprintf(ack_buf,"%c%c%c%c",0x00,ACK,0x00,0x00);
	ack_buf[2] = (block & 0xff00) >> 8;
	ack_buf[3] = (block & 0x00ff);
	printf(" After Receiving ==> Ack packet Length : %d == Opcode : %02x ---- Block number : %02x:%02x\n",packet_length,ACK,ack_buf[2],ack_buf[3]&0x00ff);
	return packet_length;
}