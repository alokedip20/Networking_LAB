#ifndef routing_h_
#define routing_h_
struct ARP{
	unsigned char iip[2];
	unsigned char mac[6];
}MY_ARP[6];
int ReadHexFile(FILE *inf, unsigned char *dest) {
  int count = 0;
  int n;
  unsigned char OneByte;
  if (dest == NULL) {
	while ((n = fscanf(inf, "%hhx", &OneByte)) == 1 ) {
	  count++;
	}
  }
  else {
	while ((n = fscanf(inf, "%hhx", dest)) == 1 ) {
	  dest++;
	}
  }
  
/*  if (n != EOF) {
	printf("%s\n","*");  // handle syntax error
  }*/
  return count;
}
void create_table(void){
	 int i,j,index,iip_index,mac_index;
	 FILE *inf;
	 int n;
	 unsigned char *hex;
	 // available hosts for netID-1 ==>1.1 & 1.3
	 //available hosts for netID-2 ==>2.1 && 2.3
	 // iip -mac mapping for 1.1    ==> client1.1
 /*    MY_ARP[0].iip[0]=0x01;
	 MY_ARP[0].iip[1]=0x01;
	 MY_ARP[0].mac[0]=0x08;
	 MY_ARP[0].mac[1]=0x00;
	 MY_ARP[0].mac[2]=0x27;
	 MY_ARP[0].mac[3]=0xd6;
	 MY_ARP[0].mac[4]=0xf4;
	 MY_ARP[0].mac[5]=0x4a;
	 //////////////////////////////
	 // iip-mac mapping for 1.2     ==>router adapter for netID-1
	 MY_ARP[1].iip[0]=0x01;
	 MY_ARP[1].iip[1]=0x02;
	 MY_ARP[1].mac[0]=0x08;
	 MY_ARP[1].mac[1]=0x00;
	 MY_ARP[1].mac[2]=0x27;
	 MY_ARP[1].mac[3]=0x93;
	 MY_ARP[1].mac[4]=0xd6;
	 MY_ARP[1].mac[5]=0x34;
	 /////////////////////////////
	 //iip-mac mapping for 2.1      ==>client2.1
	 MY_ARP[2].iip[0]=0x02;
	 MY_ARP[2].iip[1]=0x01;
	 MY_ARP[2].mac[0]=0x08;
	 MY_ARP[2].mac[1]=0x00;
	 MY_ARP[2].mac[2]=0x27;
	 MY_ARP[2].mac[3]=0x00;
	 MY_ARP[2].mac[4]=0x2b;
	 MY_ARP[2].mac[5]=0xb3;
	 ///////////////////////////////
	 //iip-mac mapping for 2.2      ==>router adapter for netID2
	 MY_ARP[3].iip[0]=0x02;
	 MY_ARP[3].iip[1]=0x02;
	 MY_ARP[3].mac[0]=0x08;
	 MY_ARP[3].mac[1]=0x00;
	 MY_ARP[3].mac[2]=0x27;
	 MY_ARP[3].mac[3]=0xe6;
	 MY_ARP[3].mac[4]=0x2e;
	 MY_ARP[3].mac[5]=0xef;
	 //////////////////////////////
	 //iip-mac mapping for 1.3
	 MY_ARP[4].iip[0]=0x01;
	 MY_ARP[4].iip[1]=0x03;
	 MY_ARP[4].mac[0]=0x08;
	 MY_ARP[4].mac[1]=0x00;
	 MY_ARP[4].mac[2]=0x27;
	 MY_ARP[4].mac[3]=0xe6;
	 MY_ARP[4].mac[4]=0x25;
	 MY_ARP[4].mac[5]=0xeb;
	 //////////////////////////////
	 //iip-mac mapping for 2.3
	 MY_ARP[5].iip[0]=0x02;
	 MY_ARP[5].iip[1]=0x03;
	 MY_ARP[5].mac[0]=0x08;
	 MY_ARP[5].mac[1]=0x00;
	 MY_ARP[5].mac[2]=0x27;
	 MY_ARP[5].mac[3]=0xa3;
	 MY_ARP[5].mac[4]=0xbb;
	 MY_ARP[5].mac[5]=0x9d;*/
	 //////////////////////////////

	 inf = fopen("routing.txt", "rt");
	 n = ReadHexFile(inf, NULL);
	 printf("%d\n",n);
	 rewind(inf);
	 hex = malloc(n);
	 ReadHexFile(inf, hex);
	 index=0;
	 iip_index=0;
	 mac_index=0;
	 for(i=0;i<n;i++){
		if(i%8!=0 && i%8!=1){
			iip_index=0;
			MY_ARP[index].mac[mac_index]=hex[i];
			if(mac_index==5){
				index++;
			}
			mac_index++;
		}
		else{
			MY_ARP[index].iip[iip_index++]=hex[i];
			mac_index=0;
		}
	 }
	 for(i=0;i<n/8;i++){
		printf("%s\n","\n IIP : ");
		for(j=0;j<2;j++){
			printf("%02x:",MY_ARP[i].iip[j]);
		}
		printf("\n MAC : ");
		for(j=0;j<6;j++){
			printf("%02x:",MY_ARP[i].mac[j]);
		}
		printf("\n");
	 }
	 if(inf!=NULL){
		printf("%s\n","SUCCESS");
	 }
	 fclose(inf);
	 free(hex);
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
		for(i=0;i<6;i++){
			if(MY_ARP[i].iip[0]==data[14]&&MY_ARP[i].iip[1]==data[15]){
				ARP_index=i;
				break;
			}
		}
		if(ARP_index>=0){
			if (data[14]==0x01){
				if(data[15]==0x02){
					printf("%s\n","This packet will be passed to default gateway" );
					return 1;
				}
				else{
					for(i=0;i<6;i++){
						data[i]=MY_ARP[ARP_index].mac[i];
						data[i+6]=MY_ADDR1[i];
					}
					send_packet(data,100,PKT_INT);
					printf("%s %02x\n","PACKET HAS COME FROM netID:2 AND WILL BE SENT TO netID:1 BY ",PKT_INT);
				}
			}
			else if(data[14]==0x02){
				if(data[15]==0x02){
					printf("%s\n","This packet will be passed to default gateway" );
					return 1;
				}
				else{
					for(i=0;i<6;i++){
						data[i]=MY_ARP[ARP_index].mac[i];
						data[i+6]=MY_ADDR2[i];
					}
					send_packet(data,100,PKT_INT2);
					printf("%s %02x\n","PACKET HAS COME FROM netID:1 AND WILL BE SENT TO netID:2 BY ",PKT_INT2);
				}
			}
		}
		else{
			printf("%s\n","No entry for coming netID found" );
		}
		return 1;
	}
}
#endif
