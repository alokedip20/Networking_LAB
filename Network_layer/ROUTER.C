#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>
unsigned char PKT_INT=0x60;
unsigned char PKT_INT2=0x70;
unsigned char CLASS[2];
unsigned char TYPE[2];
unsigned char NUMBER[2];
unsigned char flag[2];
unsigned handle[2];
unsigned bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags;
int MAX_BUFF_LEN=600;
unsigned char BUFF[600];
unsigned char BUFF2[600];
unsigned int length=0;
unsigned char PACKET[100];
unsigned int RCV1=0;
unsigned int RCV2=0;
//unsigned char MY_DEST[6]={0x08,0x00,0x27,0x00,0x2b,0xb3};
unsigned char MY_DEST[6]={0x08,0x00,0x27,0x2e,0x11,0x75};
unsigned char MY_ADDR1[6];
unsigned char MY_ADDR2[6];
unsigned int PACKET_RECEIVED=0;
unsigned int PACKET_DROPPED=0;
void create_table(void);
int routing(unsigned char *data);
struct ARP{
	unsigned char iip[2];
	unsigned char mac[6];
}MY_ARP[2];
void free_buffer(unsigned int l){
	int i;
	for(i=0;i<600;i++){
		BUFF[i]=0x00;
		BUFF2[i]=0x00;
	}
	return;
}
int filter_packet(){
	int ok=0;
	int i;
	for(i=0;i<6;i++){
		if(BUFF[i]!=MY_ADDR1[i]&&BUFF[i]!=MY_ADDR2[i]){
			ok=1;
			break;
		}
	}
	if(!ok&&(BUFF[12]!=0xab)||(BUFF[13]!=0xcd)){
		ok=1;
	}
	if(ok==0){
		PACKET_RECEIVED++;
	}
	else if(ok==1){
		PACKET_DROPPED++;
	}
	return ok;
}
void display_packet(unsigned int l){
	int i;
	if(filter_packet()==0){
		printf("%s\n",&BUFF[16]);
		if(routing(BUFF)){
			printf("%s\n","Routing has been done" );
		}
		else{
			printf("%s\n","routing not needed" );
		}
	}
	free_buffer(l);
	fflush(stdout);
	return;
}
void interrupt receiver(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags){
	int i;
	//printf("\n receiver-1 value of ax = %02x : \n",ax );
	if(ax==0){
		RCV1++;
		es=FP_SEG(BUFF);
		di=FP_OFF(BUFF);
		length=cx;
	}
	if(ax==1){
		display_packet(length);
		length=0;
	}
	return;
}
void interrupt receiver2(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags){
	int i;
	if(ax==0){
		RCV2++;
		es=FP_SEG(BUFF2);
		di=FP_OFF(BUFF2);
		length=cx;
	}
	if(ax==1){
	}
	return;
}
void get_driver_info(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	char far *p;
	a.h.ah=1;
	a.h.al=255;
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		exit(1);
	}
	if(INT==PKT_INT){
		CLASS[0]=b.h.ch;
		TYPE[0]=b.x.dx;
		NUMBER[0]=b.h.cl;
	}
	else{
		CLASS[1]=b.h.ch;
		TYPE[1]=b.x.dx;
		NUMBER[1]=b.h.cl;
	}
	printf("ver : %x\n",b.x.bx);
	printf("cls : %x\n",b.h.ch);
	printf("no : %x\n",b.h.cl);
	printf("type : %x\n",b.x.dx);
	p=MK_FP(s.ds,b.x.si);
	printf("name : %s\n",p);
	return;
}
void get_mac(unsigned char *add,unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	int i;
	a.h.ah=6;
	a.x.cx=6;
	s.es=FP_SEG(add);
	a.x.di=FP_OFF(add);
	int86x(INT,&a,&b,&s);
	printf("MY MAC ADDRESS : %d\n",b.x.cx);
	for(i=0;i<6;i++){
		printf("%02x:",*(add+i));
		if(INT==PKT_INT){
			MY_ADDR1[i]=*(add+i);
		}
		else if (INT==PKT_INT2)
		{
			MY_ADDR2[i]=*(add+i);
		}
	}
	printf("\n");
	return;

}
void access_type(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
   // printf("%x %x %x\n",CLASS,TYPE,NUMBER);
	if (INT==PKT_INT)
	{
		a.h.al=CLASS[0];
		a.x.bx=TYPE[0];
		a.h.dl=NUMBER[0];
	}
	else{
		a.h.al=CLASS[1];
		a.x.bx=TYPE[1];
		a.h.dl=NUMBER[1];
	}
	a.x.cx=0;
	a.h.ah=2;
	if(INT==PKT_INT){
		s.es=FP_SEG(receiver);
		a.x.di=FP_OFF(receiver);
	}
	else if(INT==PKT_INT2){
		s.es=FP_SEG(receiver2);
		a.x.di=FP_OFF(receiver2);
	}
	flag[0]=0x01;
	flag[1]=0x01;
	s.ds=FP_SEG(flag);
	a.x.si=FP_SEG(flag);
	int86x(INT,&a,&b,&s);
	if (INT==PKT_INT)
	{
		handle[0]=b.x.ax;
	}
	else{
		handle[1]=b.x.ax;
	}
	printf("CARRY FLAG : %x\n",b.x.cflag);
	//printf("HANDLE IN ACCESS TYPE: %x\n",handle);
	return;
}
void get_receive_mode(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=21;
	a.x.bx=(INT==PKT_INT)?handle[0]:handle[1];
	int86x(INT,&a,&b,&s);

	if(b.x.cflag){
		printf("ERROR IN RECEIVE_MODE %x\n",b.h.dh);
		exit(1);
	}
	printf("RECEIVE _MODE : %x\n",b.x.ax);
	return;
}
void set_receive_mode(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=20;
	a.x.bx=(INT==PKT_INT)?handle[0]:handle[1];
	a.x.cx=6;
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		printf("ERROR IN SET RECEIVE MODE \n");
		exit(1);
	}
	return;
}
void packet_stat(){
	printf("RECEIVED PACKET PACKET = %d\n",PACKET_RECEIVED);
	printf("PACKET REJECTED = %d\n",PACKET_DROPPED);
	printf("receiver1: %d\n",RCV1);
	printf("receiver2: %d\n",RCV2);
	return;
}
void release_type(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=3;
	a.x.bx=(INT==PKT_INT)?handle[0]:handle[1];
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		printf("ERROR IN RELEASE TYPE : \n");
		exit(1);
	}
	packet_stat();
	printf("DONE RELEASED HANDLE= =%x\n",b.x.bx);
	//exit(1);
}
void create_packet(){
	int i;
	for(i=0;i<100;i++){
		PACKET[i]=0x00;
	}
	for(i=0;i<6;i++){
		PACKET[i]=MY_DEST[i];
		PACKET[i+6]=MY_ADDR1[i];
	}
	PACKET[12]=0xab;
	PACKET[13]=0xcd;
	printf("PACKET HAS BEEN CREATED\n");
	return;
}
void send_packet(unsigned char *buffer,unsigned length,unsigned char INT){
	int i;
	union REGS a,b;
	struct SREGS s;
	a.h.ah=4;
	s.ds=FP_SEG(buffer);
	a.x.si=FP_OFF(buffer);
	a.x.cx=length;
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		printf("CAN NOT SEND PACKET\n");
		exit(1);
	}
	printf("\n SUCCESSFULLY SEND PACKET\n");
	for(i=0;i<16;i++){
		printf("%02x ",buffer[i]);
	}
	printf("\n");
	return;
}
void flush(char word[]){
	int i;
	for(i=14;i<100;i++){
		PACKET[i]=0x00;
	}
	for(i=0;i<256;i++){
		word[i]='\0';
	}
	return;
}
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
	//create_packet();
	get_driver_info(PKT_INT2);
	get_mac(my_add1,PKT_INT2);
	access_type(PKT_INT2);
	printf("HANDLE IN MAIN AFTER 2nd ACCESS_TYPE : %x\n",handle[1]);
	set_receive_mode(PKT_INT2);
	get_receive_mode(PKT_INT2);
	/*while(1){
		for(j=0;j<20;j++){
			c=getch();
			if((int)c==13){
				word[j]='\0';
				break;
			}
			word[j]=c;
			putch(word[j]);
		}
		printf("\n THE MESSAGE : %s\n",word);
		if(strncmp(word,ter,3)==0){
			break;
		}
		length=strlen(word);
		k=14;
		for(j=0;j<length;j++){
			PACKET[k++]=word[j];
		}
		PACKET[k]='\0';
		send_packet(&PACKET,100,PKT_INT);
		flush(word);
	}*/
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
	 MY_ARP[0].mac[3]=0x2e;
	 MY_ARP[0].mac[4]=0x11;
	 MY_ARP[0].mac[5]=0x75;
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
	printf("in routing : \n");
	for(i=0;i<6;i++){
			source[i]=data[i+6];
	}
	for(i=0;i<2;i++){
		flag=0;
		for(j=0;j<6;j++){
			if(MY_ARP[i].mac[j]!=source[j]){
				flag=1;
				break;
			}
		}
		if(!flag){
			netID=MY_ARP[i].iip[0];
			ARP_index=i;
			break;
		}
	}
	for(i=0;i<16;i++){
		printf("%02x ",data[i]);
	}
	if(netID==data[14]){
		printf("%s\n","Same network" );
		return 0;
	}
	else{
		printf("%s\n","different network" );
		for(i=0;i<6;i++){
			  data[i]=MY_ARP[data[14]-1].mac[i];
		}
		fflush(stdout);
		fflush(stdin);
		if (netID==0x01){
			send_packet(data,100,PKT_INT);
		}
		else if(netID==0x02){
			send_packet(data,100,PKT_INT2);
		}
		return 1;
	}
}
