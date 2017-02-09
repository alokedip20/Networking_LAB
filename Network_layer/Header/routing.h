#ifndef routing_h_
#define routing_h_
struct ARP{
	unsigned char iip[2];
	unsigned char mac[6];
}MY_ARP[2];
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
#endif