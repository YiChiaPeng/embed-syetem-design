#include<reg52.h> 
#include "ds1302.h"

#define KeyPort P3 //define button

#define DataPort P0 //define bus for 7segment displayer 
//coment
sbit LATCH1=P1^0;//DuanMA
sbit LATCH2=P1^1;//WeiMA                
sbit SPK=P1^3;          //speaker pin

bit ReadTimeFlag=1;// 1->goto read time from rs1302
bit ReadAlarmFlag=1;// 1->goto read time from rs1302
bit SetClockFlag=1;
bit SetAlarmFlag=1;//1->uart set rs1302 alarm time


unsigned char code dofly_DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//negtive
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//which digital on seg7 displayer
unsigned char TempData[8]; //put what you dispaly number on seg7 displayer

/*------------------------------------------------
        declare the function
--------------------------------------------------*/
void DelayUs2x(unsigned char t);//us
void DelayMs(unsigned char t); //ms
void Display(unsigned char FirstBit,unsigned char Num);//output the data onto 7segment displayer
unsigned char KeyScan(void);//handle which button be pressed
void Init_Timer0(void);//

void UART_Init(void);
void putNote();
unsigned char Timer0_H,Timer0_L,Time;//buffer of TH0 and TL0

/*------------------------------------------
                the song 
--------------------------------------------*/
code unsigned char MUSIC[]={          6,2,3,      5,2,1,      3,2,2,    5,2,2,    1,3,2,    6,2,1,    5,2,1,
                                      6,2,4,      3,2,2,      5,2,1,    6,2,1, 	  5,2,2, 	3,2,2, 	  1,2,1,
                                      6,1,1,      5,2,1,      3,2,1, 	2,2,4, 	  2,2,3, 	3,2,1,    5,2,2,
                                      5,2,1,      6,2,1,      3,2,2, 	2,2,2,    1,2,4, 	5,2,3, 	  3,2,1,
                                      2,2,1,      1,2,1,      6,1,1, 	1,2,1, 	  5,1,6, 	0,0,0 
                                      };
                         // “ÙΩ◊∆µ¬ ±Ì ∏ﬂ∞ÀŒª
code unsigned char FREQH[]={
                                0xF2,0xF3,0xF5,0xF5,0xF6,0xF7,0xF8, 
                                0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,
                                0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,0xFE,
                                0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,
                               } ;
                         // “ÙΩ◊∆µ¬ ±Ì µÕ∞ÀŒª
code unsigned char FREQL[]={
                                 0x42,0xC1,0x17,0xB6,0xD0,0xD1,0xB6,
                                 0x21,0xE1,0x8C,0xD8,0x68,0xE9,0x5B, //1,2,3,4,5,6,7,8,i
                                 0x8F,0xEE,0x44, 0x6B,0xB4,0xF4,0x2D, 
                                 0x47,0x77,0xA2,0xB6,0xDA,0xFA,0x16,
                                };

/*--------------------------------------------------------
function to play song
-----------------------------------------------------------*/
void Song(void)
{  
    unsigned char k,i;
    Init_Timer0();
    while(i<100)
	{         //�������鳤�� �������ͷ����        
            k=MUSIC[i]+7*MUSIC[i+1]-1;//ȥ������Ƶ����������
            Timer0_H=FREQH[k];
            Timer0_L=FREQL[k];
            Time=MUSIC[i+2];          //����ʱ��
            i=i+3;
            putNote();
           
    } 
   ReadTimeFlag=1;
   ReadAlarmFlag=1;
}
void delay(unsigned char t)
{
    unsigned char i;
	for(i=0;i<t;i++)
	    DelayMs(250);
    TR0=0;
 }
/*-----------------------------------------------
function to load correspond note counting time into timer0
------------------------------------------------*/
void putNote()
{
    TH0=Timer0_H;//��ֵ��ʱ��ʱ�䣬����Ƶ��
    TL0=Timer0_L;
    TR0=1;       //�򿪶�ʱ��
    delay(Time); //��ʱ����Ҫ�Ľ���     
}
/*-----------------------------------------
    to chech now is  o'clock 
------------------------------------------*/
bit is_sharp()
{
    //whether minute and sec all equal 0
    //mean a clock
    if(time_buf1[5]==0&&time_buf1[6]==0)
        return 1;
}
/*------------------------------------------------
                main
------------------------------------------------*/

void main (void)
{
	unsigned char i;
    unsigned char num,displaynum;
    unsigned int cnt;    //by kept after how many instruction done to counting time then updated time from ds1302
    //Init_Timer0();
    UART_Init();
    Ds1302_Init();


    while (1) //always loop
    {   
        
        cnt+=10;
        /*-----------------------------------------------------
            handling uart set clk time
        -----------------------------------------------------*/  
        if(SetClockFlag)     //to set clk time
	    {
			for(i=0;i<8;i++)
			{
				time_buf1[i]=time_buf2[2*i]*10+time_buf2[2*i+1];
			}//to combine digital to 
			Ds1302_Write_Time();
			SetClockFlag=0;       //release signal
		}
        /*-----------------------------------------------------
            handling uart set alarm time
        -----------------------------------------------------*/  
        if(SetAlarmFlag)     //to set clk time
	    {
			for(i=0;i<3;i++)
			{
				alarm_buf1[i]=time_buf2[2*i]*10+time_buf2[2*i+1];
			}//to combine digital to 
			Ds1302_Alarm_Write_Time();
			SetAlarmFlag=0;       //release signal
		}
		/*-----------------------------------------------------
            handling keyboard
        -----------------------------------------------------*/  
        num=KeyScan();   //num return from keyboard
        if(num==1)
        {
            displaynum++;
            //decide which mode on 7segment displayer
            if(displaynum==4)
                displaynum=0;
        }
        /*---------------------------------------------------
                whether updated time from ds1302
        -----------------------------------------------------*/
        //suppose a num
        if (cnt>2000)
        {
            ReadTimeFlag=1;
            ReadAlarmFlag=1;
            cnt=0;
        }
        
        //it suppose to get time from ds1302
        if(ReadTimeFlag==1)
        {
            ReadTimeFlag=0;
            //getting the time from ds1302
            Ds1302_Read_Time();
            if(displaynum==0) //display time on the 7segment displayer
            {
                 TempData[0]=dofly_DuanMa[time_buf1[4]/10];//gain the hour's lower digital    ex 15 its 5
                 TempData[1]=dofly_DuanMa[time_buf1[4]%10];//gain the hour's higher digital ex 15 its 1
                 TempData[2]=0x40;                                    //display"-"
                 TempData[3]=dofly_DuanMa[time_buf1[5]/10];//gain the minute's lower digital    ex 15 its 5
                 TempData[4]=dofly_DuanMa[time_buf1[5]%10];//gain the minute's lower digital    ex 15 its 5
                 TempData[5]=0x40;
                 TempData[6]=dofly_DuanMa[time_buf1[6]/10];//gain the sec's lower digital    ex 15 its 5
                 TempData[7]=dofly_DuanMa[time_buf1[6]%10];//gain the sec's lower digital    ex 15 its 5
            }
            else if(displaynum==1)//display year-month-day on the 7segment displayer
            {
                 TempData[0]=dofly_DuanMa[time_buf1[1]/10];//��
                 TempData[1]=dofly_DuanMa[time_buf1[1]%10];
                 TempData[2]=0x40;                                    //����"-"
                 TempData[3]=dofly_DuanMa[time_buf1[2]/10];//��
                 TempData[4]=dofly_DuanMa[time_buf1[2]%10];
                 TempData[5]=0x40;
                 TempData[6]=dofly_DuanMa[time_buf1[3]/10];//��
                 TempData[7]=dofly_DuanMa[time_buf1[3]%10];
            }
            else if(displaynum==2)//display week and seconds on the 7segment displayer
            {
                TempData[0]=0x40;    //display"-"
                TempData[1]=dofly_DuanMa[time_buf1[7]/10];//gain the week's  digital    ex Friday  its 5
                TempData[2]=dofly_DuanMa[time_buf1[7]%10];    //display"-"
                TempData[3]=0x40;;
                TempData[4]=0;
                TempData[5]=0;//display nothing on the 5th digital of 7segment displayer
                TempData[6]=dofly_DuanMa[time_buf1[6]/10];//gain the sec's lower digital    ex 15 its 5
                TempData[7]=dofly_DuanMa[time_buf1[6]%10];//gain the sec's lower digital    ex 15 its 5
            }
        }
        if (ReadAlarmFlag==1)
        {
            ReadAlarmFlag=0;
            Ds1302_Alarm_Read_Time();
            if(displaynum==3) //display time on the 7segment displayer
            {
                TempData[0]=dofly_DuanMa[alarm_buf1[0]/10];//gain the hour's lower digital    ex 15 its 5
                TempData[1]=dofly_DuanMa[alarm_buf1[0]%10];//gain the hour's higher digital ex 15 its 1
                TempData[2]=0x40;                                    //display"-"
                TempData[3]=dofly_DuanMa[alarm_buf1[1]/10];//gain the minute's lower digital    ex 15 its 5
                TempData[4]=dofly_DuanMa[alarm_buf1[1]%10];//gain the minute's lower digital    ex 15 its 5
                TempData[5]=0x40;
                TempData[6]=dofly_DuanMa[alarm_buf1[2]/10];//gain the sec's lower digital    ex 15 its 5
                TempData[7]=dofly_DuanMa[alarm_buf1[2]%10];//gain the sec's lower digital    ex 15 its 5
            }

        }
        /*----------------------------------------------------
        to display the DuanMa value of Temp on  7 segment displayer
        -----------------------------------------------------*/
        Display(0,8);
        /*-----------------------------------------------------
            handling o'clock
            to play song
        -----------------------------------------------------*/  
        if(is_sharp()){
            Song();
        }
         /*-----------------------------------------------------
            handling the alarm ring
        -----------------------------------------------------*/ 
        if (alarm_buf1[0]==time_buf1[4]&&alarm_buf1[1]==time_buf1[5]&&alarm_buf1[2]==time_buf1[6])
        {
            Song();
        }
        
    }
}
/*------------------------------------------------

------------------------------------------------*/
void DelayUs2x(unsigned char t)
{
 while(--t);
}
/*------------------------------------------------

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
 start display from FirstBit digital  to Num of 7seg displayer
------------------------------------------------*/
void Display(unsigned char FirstBit,unsigned char Num)
{
    unsigned char i=FirstBit;
    while(i<Num)
    {
        DataPort=0;   //display all black
        LATCH1=1;     //
        LATCH1=0;

        DataPort=dofly_WeiMa[i]; //output which position
        LATCH2=1;     //
        LATCH2=0;

        DataPort=TempData[i]; //output
        LATCH1=1;     //������
        LATCH1=0;
       
        i++;
        DelayUs2x(5);

    }

}
/*------------------------------------------------
            KeyScan
------------------------------------------------*/
unsigned char KeyScan(void)
{
 unsigned char keyvalue;
 if(KeyPort!=0xff)//some button be pressed
   {
    DelayMs(10);//debounce
    if(KeyPort!=0xff)//if you really pressed
       {
        keyvalue=KeyPort;
        while(KeyPort!=0xff);
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

/*------------------------------------------------
                Timer 0 initial
Timer controll the speaker
------------------------------------------------*/
void Init_Timer0(void)
{
    TMOD |= 0x01;      //using timer mode 1 16bit counter to count time
    //TH0=0x00;      //assign initial value
    //TL0=0x00;
    EA=1;            //enable total interrupt
    ET0=1;           //enable interupt from timer
    TR0=1;           //turn off timer0 to counting time
 
}

/*------------------------------------------------
        Timer0 interupt service program
        to handle speaker
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
 	TR0=0;      
 	SPK=!SPK;
	TH0=Timer0_H;
 	TL0=Timer0_L;
 	TR0=1;
}
/*------------------------------------------------
                Uart init
uart use the timer1 to generate bault rate 
------------------------------------------------*/
void UART_Init(void)
{
    SCON  = 0x50;		        // SCON: 0101_0000
                                //mode 1
                                //SM0,SM1 reprsent mode  
                                //REN=1
                                //RI=1 while some date transfered to the SBUF
                                //TI=1 while SBUF=null
                                //SMOD baudrate double or not 
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit auto reload count
    TH1   = 0xFD;               // TH1:  ��װֵ 9600 ������ ���� 11.0592MHz  253
    TR1   = 1;                  // TR1:  timer 1 enable start counting                         
    EA    = 1;                  //enable total interrupt
    ES    = 1;                  //�򿪴����ж�
}
/*------------------------------------------------
                UART isr
------------------------------------------------*/
void UART_SER (void) interrupt 4 
{	unsigned j;
    unsigned char Temp;          //place raw data 
    static unsigned char i;
    static bit ins;             //to kept what ins
    static unsigned char len;   //to adjust the inst len
    if(RI)                        //receive some thing
    {
	  	RI=0;                      	//clear the signal of receive some thing
	  	Temp=SBUF;                 	//get the raw value 
        //the C type instruction mean setting the clock time 
        if (Temp=='C')
        {
            ins=0;
            //start counting the following 16 byte input
			i=0;
            len=16;//ex 2022_0620_1104_3501
		
        }
        else if (Temp=='A')
        {
            ins=1;
            //start counting the following 6 byte input
			i=0;
            len=6;//ex 1115_20 
        }
        //mean receive number
        else if(Temp<='9'&&Temp>='0')
        {
            time_buf2[i]=Temp&0x0F;		//translate assci to demical
		    i++;
	  	    if(i==len)                  //it completly received 16 or 6 byte
		    {
	    	    i=0;
			    
                if (ins==0)
                {
                    //to avoid there are two instruction want to be process
                    SetAlarmFlag=0;
                    SetClockFlag=1;//1->update the ds1302 time 
                }else{
                    SetAlarmFlag=1;
                    SetClockFlag=0;
                }
                             
	   	    }
        }
	  	
      SBUF=Temp; //return what you receive to the sender
	
	}
   	if(TI)  //check whether can send
    	TI=0;
} 


