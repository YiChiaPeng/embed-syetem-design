#include<reg52.h> 
#include "ds1302.h"

#define KeyPort P3 //Keyport
#define KeyMatrixPort P2 //Matrix Keyport
#define DataPort P2 //for segment7 displayer

sbit LATCH1=P1^0;//controll display number
sbit LATCH2=P1^1;//controll on which digital

sbit SPK=P1^2;//control the speaker

bit ReadTimeFlag;//if true read clk time form DS1302
bit SetFlag;     //����ʱ���־λ

typedef unsigned char byte;
typedef unsigned int  word;

byte code dofly_DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};// ��ʾ����ֵ0~9
byte code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//�ֱ��Ӧ��Ӧ������ܵ���,��λ��
byte TempData[8]; //the clock time temp buffer

//uart receive buffer
byte buf[MAX];
byte tail = 0;
byte head = 0;
byte ms = 0;

//using the timer 0 to  interupt to display

void DelayUs2x(unsigned char t);//us����ʱ�������� 
void DelayMs(unsigned char t); //ms����ʱ
void Display(unsigned char FirstBit,unsigned char Num);//�������ʾ����
unsigned char KeyScan(void);//
void Init_Timer0(void);//init the timer0
/*------------------------------------------------
                    ������
------------------------------------------------*/
void main (void)
{
	unsigned char i,num,displaynum;
	unsigned char k=0;
	byte j=0;

	cnt=0;                 
	Init_Timer0();
	Ds1302_Init();
	UART_Init();
	//in the while 1
	//handle 3 part 
	//first part uart handler 
	while (1) //always loop
	{
		

		//deal uart instruction
		//the queue is not empty
		if(SetFlag)     //������յ�������Ϣ�����ʱ��
			  {
				for(i=0;i<8;i++)
					{
					time_buf1[i]=time_buf2[2*i]*10+time_buf2[2*i+1];
					}//�������ϣ���2���� 1��5���ϳ�15
					Ds1302_Write_Time();
					SetFlag=0;       //ʱ����Ϣ���º��־λ����
			   }
			
			 num=KeyScan();
			if(num==1)
			  {
				   displaynum++;
				   if(displaynum==3)
				      displaynum=0;
			  }
			if(ReadTimeFlag==1)
			{
				  ReadTimeFlag=0;
				  Ds1302_Read_Time();
				if(displaynum==0) //��ʾʱ��
				 {
					 TempData[0]=dofly_DuanMa[time_buf1[4]/10];//ʱ			//���ݵ�ת���������ǲ��������0~9����ʾ,�����ݷֿ�
					 TempData[1]=dofly_DuanMa[time_buf1[4]%10];
					 TempData[2]=0x40;					    	        //����"-"
					 TempData[3]=dofly_DuanMa[time_buf1[5]/10];//��
					 TempData[4]=dofly_DuanMa[time_buf1[5]%10];
					 TempData[5]=0x40;
					 TempData[6]=dofly_DuanMa[time_buf1[6]/10];//��
					 TempData[7]=dofly_DuanMa[time_buf1[6]%10];	
				 } 
				else if(displaynum==1)//��ʾ����
				 { 
					 TempData[0]=dofly_DuanMa[time_buf1[1]/10];//��			//���ݵ�ת���������ǲ��������0~9����ʾ,�����ݷֿ�
					 TempData[1]=dofly_DuanMa[time_buf1[1]%10];
					 TempData[2]=0x40;					    	        //����"-"
					 TempData[3]=dofly_DuanMa[time_buf1[2]/10];//��
					 TempData[4]=dofly_DuanMa[time_buf1[2]%10];
					 TempData[5]=0x40;
					 TempData[6]=dofly_DuanMa[time_buf1[3]/10];//��
					 TempData[7]=dofly_DuanMa[time_buf1[3]%10];	
				 }
				else if(displaynum==2)//��ʾ��  ��
				 {
					 TempData[0]=0x40;				//���ݵ�ת���������ǲ��������0~9����ʾ,�����ݷֿ�
					 TempData[1]=dofly_DuanMa[time_buf1[7]%10];//��
					 TempData[2]=0x40;	//����"-"
					 TempData[3]=0;
					 TempData[4]=0;
					 TempData[5]=0;
					 TempData[6]=dofly_DuanMa[time_buf1[6]/10];//��
					 TempData[7]=dofly_DuanMa[time_buf1[6]%10];	
				 }



		//if cnt out the range of start~end 
		//reset it	
		//deal keyboard
		num=KeyScan();
		
		switch(num)
		{
			case 1:time_buf1[4]++;if(time_buf1[4]==24)time_buf1[4]=0;    
		        Ds1302_Write_Time();break; //����ʱ�� Сʱ ��1
			case 2:time_buf1[4]--;if(time_buf1[4]==255)time_buf1[4]=23;  
			    Ds1302_Write_Time();break; //����ʱ�� Сʱ��1
			case 3:time_buf1[5]++;if(time_buf1[5]==60)time_buf1[5]=0;     
			    Ds1302_Write_Time();break;//�ּ�1
			case 4:time_buf1[5]--;if(time_buf1[5]==255)time_buf1[5]=59; 
			    Ds1302_Write_Time();break; //�ּ�1
			default:break;
		}
		if(ReadTimeFlag==1)
		{
			ReadTimeFlag=0;
			Ds1302_Read_Time();
			 //���ݵ�ת���������ǲ��������0~9����ʾ,�����ݷֿ�
			 TempData[0]=dofly_DuanMa[time_buf1[4]/10];	//ʱ		
			 TempData[1]=dofly_DuanMa[time_buf1[4]%10];
			 TempData[2]=0x40;					    	        //����"-"
			 TempData[3]=dofly_DuanMa[time_buf1[5]/10]; //��
			 TempData[4]=dofly_DuanMa[time_buf1[5]%10];
			 TempData[5]=0x40;
			 TempData[6]=dofly_DuanMa[time_buf1[6]/10]; //��
			 TempData[7]=dofly_DuanMa[time_buf1[6]%10];	
		
		}	
  	}
}
/*------------------------------------------------
 uS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��,������ʱ
 �������� T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //������ʱ1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*------------------------------------------------
			
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
      static unsigned char i=0;
	  

	   DataPort=0;   //������ݣ���ֹ�н�����Ӱ
       LATCH1=1;     //������
       LATCH1=0;

       DataPort=dofly_WeiMa[i+FirstBit]; //ȡλ�� 
       LATCH2=1;     //λ����
       LATCH2=0;

       DataPort=TempData[i]; //ȡ��ʾ���ݣ�����
       LATCH1=1;     //������
       LATCH1=0;
       
	   i++;
       if(i==Num)
	      i=0;


}


void Init_Timer0(void)
{
 TMOD |= 0x01;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
 //TH0=0x00;	      //������ֵ
 //TL0=0x00;
 EA=1;            //���жϴ�
 ET0=1;           //��ʱ���жϴ�
 TR0=1;           //��ʱ�����ش�
}
/*----------------------------------
		Uart_init
----------------------------------------*/
void UART_Init(void)
{
    SCON  = 0x50;		        // SCON: ģʽ 1, 8-bit UART, ʹ�ܽ���  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit ��װ
    TH1   = 0xFD;               // TH1:  ��װֵ 9600 ������ ���� 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 ��                         
    EA    = 1;                  //�����ж�
    ES    = 1;                  //�򿪴����ж�
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
	

/*------------------------------------------------
                Init the timer0
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //using the timer mode 1 , 16bit counter     
 //TH0=0x00;	      //initial counter higher 8 bit
 //TL0=0x00;
 EA=1;            //turn on the interupt
 ET0=1;           //turn on the timer0 interupt
 TR0=1;           //tutrn up timer0 
}
/*------------------------------------------------
                Timer0 isr
				using the timer 0 's  interrupt to 
				display one digital segment displayer
				at thte  frequenecy of every 2 ms 
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
	static unsigned int num;		//declare num is not local varible
 	TH0=(65536-2000)/256;		  	//reassign 2ms
 	TL0=(65536-2000)%256;
 
	Display(0,8);       //scan the segment 7 displayer
 	num++;
 	if(num==50)        //about 100ms
   	{
    	num=0;
    	ReadTimeFlag=1; //when ReadTimeFlag==1
						//it will read out the time os Ds1302 
	}
}

/*------------------------------------------------
		KeyBoard
------------------------------------------------*/
unsigned char KeyScan(void)
{
	
	unsigned char keyvalue;
	
	if(KeyPort!=0xff)//you have pressed any key
   	{	
		
    	DelayMs(10);//debounce
    	if(KeyPort!=0xff)//if you really pressed any botton
	   	{
	    	keyvalue=KeyPort;
	    	while(KeyPort!=0xff);//wait until you release
			
			switch(keyvalue)
			{
		 		case 0xfe:return 1;break;
		 		case 0xfd:return 2;break;
				case 0xfb:return 3;break;
		 		case 0xf7:return 4;break;
		 		case 0xef:return 5;break;
		 		case 0xdf:return 6;break;
		 		case 0xbf:return 7;break;
		 		case 0x7f:return 8;break;
		 		default:return 0;break;
			}
		}
	}
   	return 0;
}

