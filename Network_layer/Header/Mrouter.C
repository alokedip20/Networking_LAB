//NET ID 2 WILL BE CONNECTED WITH USB ETHERNET
//NET ID 1 WILL BE CONNECTED WITH PCI ETHERNET
#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include "my_net.h"
void create_table(void);
int routing(unsigned char *data);
struct ARP{
	unsigned char iip[2];
	unsigned char mac[6];
}MY_ARP[2];
void main(){
	unsigned char *my_add1=(unsigned char *)malloc(6);
	unsigned char *my_add2=(unsigned char *)malloc(6);
	char word[256];
	char ter[4]="BYE";
	char c;
	int length,k,j;
	create_table();
	get_driver_info(PKT_INT);
	get_mac(my_add1,PKT_INT);
	access_type(PKT_INT);
	printf("HANDLE IN MAIN AFTER 1ST ACCESS_TYPE : %x\n",handle[0]);
	set_receive_mode(PKT_INT);
	get_receive_mode(PKT_INT);
	get_driver_info(PKT_INT2);
	get_mac(my_add2,PKT_INT2);
	access_type(PKT_INT2);
	printf("HANDLE IN MAIN AFTER 2nd ACCESS_TYPE : %x\n",handle[1]);
	set_receive_mode(PKT_INT2);
	get_receive_mode(PKT_INT2);
	getch();
	release_type(PKT_INT);
	release_type(PKT_INT2);
}
void create_table(void){
	 int i,j;
	 MY_ARP[0].iip[0]=0x01;
	 MY_ARP[0].iip[1]=0x01;
	 MY_ARP[0].mac[0]=0x08;
	 MY_ARP[0].mac[1]=0x00;
	 MY_ARP[0].mac[2]=0x27;
	 MY_ARP[0].mac[3]=0xd6;
	 MY_ARP[0].mac[4]=0xf4;
	 MY_ARP[0].mac[5]=0x4a;
	 //////////////////////////////
	 MY_ARP[1].iip[0]=0x02;
	 MY_ARP[1].iip[1]=0x01;
	 MY_ARP[1].mac[0]=0x08;
	 MY_ARP[1].mac[1]=0x00;
	 MY_ARP[1].mac[2]=0x27;
	 MY_ARP[1].mac[3]=0x00;
	 MY_ARP[1].mac[4]=0x2b;
	 MY_ARP[1].mac[5]=0xb3;
	 ///////////////////////////////
	 for(i=0;i<2;i++){
		for(j=0;j<2;j++){
			printf("%02x:",MY_ARP[i].iip[j]);
		}
		printf("\n");
		for(j=0;j<6;j++){
			printf("%02x:",MY_ARP[i].mac[j]);
		}
		printf("\n");
	 }
}
int routing(unsigned char *data){
	unsigned char source[6];
	unsigned char netID;
	int i,j,flag,ARP_index;
	ARP_index=-1;
	printf("in routing : \n");
	netID=data[16];
	printf("%s\n","ROUTER HAS RECEIVED PACKET DATA : ");
	for(i=0;i<16;i++){
		printf("%02x ",data[i]);
	}
	printf("\n");
	if(netID==data[14]){
		printf("%s\n","Same network" );
		return 0;
	}
	else{
		printf("%s\n","different network" );
		fflush(stdout);
		fflush(stdin);
		for(i=0;i<2;i++){
			if(MY_ARP[i].iip[0]==data[14]&&MY_ARP[i].iip[1]==data[15]){
				ARP_index=i;
				break;
			}
		}
		if(ARP_index>=0){
			if (data[14]==0x01){
				for(i=0;i<6;i++){
					data[i]=MY_ARP[ARP_index].mac[i];
					data[i+6]=MY_ADDR1[i];
				}
				send_packet(data,100,PKT_INT);
				printf("%s %02x\n","PACKET HAS COME FROM netID:2 AND WILL BE SENT TO netID:1 BY ",PKT_INT);
			}
			else if(data[14]==0x02){
				for(i=0;i<6;i++){
					data[i]=MY_ARP[ARP_index].mac[i];
					data[i+6]=MY_ADDR2[i];
				}
				send_packet(data,100,PKT_INT2);
				printf("%s %02x\n","PACKET HAS COME FROM netID:1 AND WILL BE SENT TO netID:2 BY ",PKT_INT2);
			}
		}
		else{
			printf("%s\n","No entry for coming netID found" );
		}
		return 1;
	}
}
