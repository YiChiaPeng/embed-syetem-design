#include <reg52.h>

#define  BUS  P0
unsigned char code font[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90}; 
unsigned char code digit[8]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
unsigned char code hello[] = {"HELLO\r\n"};

#define ms50 65536-46080  // 11.0592Mhz
#define ms10 65536-10000
#define MAX 10

typedef unsigned char byte;
typedef unsigned int  word;

sbit fontcs = P2^0;
sbit digitcs = P2^1;

unsigned long start=0;//a number counter start counting from  
unsigned long end=100000000;// 

unsigned char sec=1; // every s sec cnt++
unsigned char state=0;
byte instruction_type =2;//default countup
						//0->F,1->T,U->2,T->3,S->4

unsigned long cnt;
unsigned long pre_cnt;

byte buf[MAX];
byte tail = 0;
byte head = 0;
byte ms = 0;

void timer_init()  //timer0
{
	TMOD = TMOD | 0x01;
	TH0 = ms50/256;
	TL0 = ms50%256;
	TF0 = 0;
	
	TR0 = 1;
    EA = 1;
	ET0 = 1;

}

void uart_init()
{
	SCON = 0x50;  // mode 1, 嚙褕踝蕭嚙踝蕭
//	PCON = 0x0; //SMOD = 0;

    TMOD = TMOD | 0x20; //mode 2, timer1
	TH1 = 0xfd; //9600, 11.0592MHz
	TR1 = 1;
	RI = 0;
	TI = 0;

    EA  = 1;
	ES = 1;
}

void uart_isr() interrupt 4 //uart interrupt
{
byte i;
	//it can read
	if (RI == 1)
	{
		RI = 0;
		i = SBUF;
		
		
		if(i=='F')
		{
			instruction_type=0;
			//Send  char F back
			SBUF = i;
			while(TI != 1);
			TI = 0;
		}else if(i=='T'){
			instruction_type=1;
			//Send  char T back
			SBUF = i;
			while(TI != 1);
			TI = 0;
		}
		//the instruction with out /c/r(enter)
		else if(i=='U')
		{
			instruction_type=2;
			state=0;
			//clear queue
			tail = 0;
			head = 0;
			pre_cnt = 0;
			
			//Send  char F back
			SBUF = i;
			while(TI != 1);
			TI = 0;
			//change row
			SBUF = '\r';
			while(TI != 1);
			TI = 0;
			SBUF = '\n';
			while(TI != 1);
			TI = 0;
			
		}else if (i=='D')
		{		
			instruction_type=3;
			state=1;
			//clear queue
			tail = 0;
			head = 0;
			pre_cnt = 0;
			
			//Send  char D back
			SBUF = i;
			while(TI != 1);
			TI = 0;

			//change row
			SBUF = '\r';
			while(TI != 1);
			TI = 0;
			SBUF = '\n';
			while(TI != 1);
			TI = 0;
			
		}else if(i=='S')
		{
			instruction_type=4;
			//Send  char F back
			SBUF = i;
			while(TI != 1);
			TI = 0;
		}
		else
		{	buf[tail] = i;
			tail++;
			if (tail == MAX) tail = 0;
			//s instuction end up with receiving a one digital demical number
			if(instruction_type==4){
				//i is a ascci
				sec=i-0x30;
				//clear queue
				tail = 0;
				head = 0;
				pre_cnt = 0;
				//
				//Send  char i back
				SBUF = i;
				while(TI != 1);
				TI = 0;				
				//change row
				SBUF = '\r';
				while(TI != 1);
				TI = 0;
				SBUF = '\n';
				while(TI != 1);
				TI = 0;
			}
		}
	}	
}


void timer_isr() interrupt 1 //timer0 interrupt
{

		TH0 = ms50/256;
		TL0 = ms50%256;
		TF0 = 0;
			ms++;
			//20 as 1 second, than 
			if (ms == 20*sec)
			{
				ms = 0;
				//decide countup or count down
				if(state==0)
					cnt++;
				if(state==1)
					cnt--;
			}	
}

void delay(unsigned int d)
{
unsigned int i,j;
	for(i=0;i<10;i++)
		for(j=0;j<d;j++);
}

void put_data(unsigned char i,unsigned char j)
{
	BUS = 0xff - font[j]; 
	fontcs = 1;
	fontcs = 0;

	BUS = digit[i];
	digitcs = 1;
	digitcs = 0;
}

void display(unsigned long i)
{
unsigned char j;


	for(j=0;j<8;j++)
	{
		put_data(7-j,i%10);
		delay(5);
		i=i/10;
	}

}		

void send_cnt()
{
	unsigned long temp;
	byte i,j;
	temp = cnt;
	i=0;
	while(temp != 0)
	{
		buf[i] = (temp%10)+0x30;
		temp/=10;
		i++;

	}
	for(j=i;j>0;j--)
	{
		SBUF = buf[j-1];
		while(TI != 1);
		TI = 0;
	
	}
	//change row
	SBUF = '\r';
	while(TI != 1);
	TI = 0;
	SBUF = '\n';
	while(TI != 1);
	TI = 0;

}			
		
void test_send()
{
byte i;
	for(i=0;i<7;i++)
	{
		SBUF = hello[i];
		while(TI != 1);
		TI = 0;
	}	
}
	
void main()
{

	unsigned char i;	
	unsigned char k=0;
	byte j=0;

	cnt=0;
	timer_init();
	uart_init();
	test_send();
	while(1)
	{	//the queue is not empty
		if (head != tail)
		{   
			//process F or T instruction
			//the instruction must end up with '\r' 	
			if (buf[head] == '\r')  //CR ASCii = 0x0D,   \n: 0x0a
			{
				
				if (instruction_type==0)
				{
					//show that number
					cnt = pre_cnt;
					//change the head of count
					start=pre_cnt;
					pre_cnt = 0;
				}else if(instruction_type==1)
				{
					//show that number
					cnt = pre_cnt;
					//change the head of count
					end=pre_cnt;
					pre_cnt = 0;
				}
				
				head++;
				if (head == MAX) head = 0;
				send_cnt();
			}
			else
			{
				pre_cnt = pre_cnt * 10 + (buf[head]-0x30);  //'0':0x30, '1': 0x31
				head++;
				if (head == MAX) head = 0;
			}
		}	
		//if cnt out the range of start~end 
		//reset it
		if(cnt>end)
			cnt=start;
		if(cnt<start)
			cnt=end;
		display(cnt);

	}
}
		
		



