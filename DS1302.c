#include "ds1302.h"
//to kept the alarm time
unsigned char alarm_buf1[3];//demecial
unsigned char alarm_buf[3];//BCD
//alarm_buf1[0]->hour
//alarm_buf1[1]->min
//alarm_buf1[2]->sec
unsigned char time_buf2[16] ;
unsigned char time_buf1[8];//demecial
unsigned char time_buf[8] ;                         //BCD
/*------------------------------------------------
           ��DS1302д��һ�ֽ�����
------------------------------------------------*/
void Ds1302_Write_Byte(unsigned char addr, unsigned char d)
{

	unsigned char i;
	RST_SET;	
	
	//write is first write addr into rs1302
	//then write data into rs1302 
	//there had been declared into write addr in the DS1302.h
	
	for (i = 0; i < 8; i ++) 
	    { 
		if (addr & 0x01) 
		    {
			IO_SET;
			}
		else 
		    {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;
		}
	
	//д�����ݣ�d
	for (i = 0; i < 8; i ++) 
	   {
		if (d & 0x01) 
		    {
			IO_SET;
			}
		else 
		    {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		d = d >> 1;
		}
	RST_CLR;					//ֹͣDS1302����
}
/*------------------------------------------------
           ��DS1302����һ�ֽ�����
------------------------------------------------*/

unsigned char Ds1302_Read_Byte(unsigned char addr) 
{

	unsigned char i;
	unsigned char temp;
	RST_SET;	

	//your instruction are store in addr
	//change yor instruction into read
	addr = addr | 0x01;
	for (i = 0; i < 8; i ++) 
	{
	    //fetch out the lsb
		if (addr & 0x01) 
		{
			IO_SET;
		}
		else 
		{
			IO_CLR;
		}
		//generating clk
		SCK_SET;
		SCK_CLR;
		//shift the addr
		addr = addr >> 1;
	}

	//after the inputing addr
	//data will come out
	//catch the data and put the data into temp 
	for (i = 0; i < 8; i ++) 

	{
		//the shift right temp 
		temp = temp >> 1;
		//the first readed bit is the lsb
		//put they into msb of temp
		if (IO_R) 
		{
			temp |= 0x80;
		}
		else 
		{
			temp &= 0x7F;
		}
		SCK_SET;
		SCK_CLR;
	}
	
	RST_CLR;	//end DS1302
	return temp;
}

/*------------------------------------------------
        write time to DS1302
------------------------------------------------*/
void Ds1302_Write_Time(void) 
{
     
    unsigned char i,tmp;
	//binary number to BCD number
	for(i=0;i<8;i++)
	{                 
		tmp=time_buf1[i]/10;
		time_buf[i]=time_buf1[i]%10;
		time_buf[i]=time_buf[i]+tmp*16;
	}
	Ds1302_Write_Byte(ds1302_control_add,0x00);			//close write controll protect-> set to 0
	Ds1302_Write_Byte(ds1302_sec_add,0x80);				//sec msb set 0 to stop counting
	//Ds1302_Write_Byte(ds1302_charger_add,0xa9);		//tiny current to charge
	Ds1302_Write_Byte(ds1302_year_add,time_buf[1]);		//year
	Ds1302_Write_Byte(ds1302_month_add,time_buf[2]);	//month 
	Ds1302_Write_Byte(ds1302_date_add,time_buf[3]);		//date 
	Ds1302_Write_Byte(ds1302_hr_add,time_buf[4]);		//hour
	Ds1302_Write_Byte(ds1302_min_add,time_buf[5]);		//min
	Ds1302_Write_Byte(ds1302_sec_add,time_buf[6]);		//sec
	Ds1302_Write_Byte(ds1302_day_add,time_buf[7]);		//week 
	Ds1302_Write_Byte(ds1302_control_add,0x80);			//reopen write controll protect
}

/*------------------------------------------------
           DS1302_Read_time from 
------------------------------------------------*/
void Ds1302_Read_Time(void)  
{ 
   	unsigned char i,tmp;
	time_buf[1]=Ds1302_Read_Byte(ds1302_year_add);		//year
	time_buf[2]=Ds1302_Read_Byte(ds1302_month_add);		//month
	time_buf[3]=Ds1302_Read_Byte(ds1302_date_add);		//date
	time_buf[4]=Ds1302_Read_Byte(ds1302_hr_add);		//hour
	time_buf[5]=Ds1302_Read_Byte(ds1302_min_add);		//minute
	time_buf[6]=(Ds1302_Read_Byte(ds1302_sec_add))&0x7F;//sec
														//sec's msb is represent the counting or not 
	time_buf[7]=Ds1302_Read_Byte(ds1302_day_add);		//week


	for(i=0;i<8;i++)
	   {           //BCD translate to demical
		tmp=time_buf[i]/16;
		time_buf1[i]=time_buf[i]%16;
		time_buf1[i]=time_buf1[i]+tmp*10;
	   }
}


/*------------------------------------------------
        Write Alarm time to DS1302
------------------------------------------------*/
void Ds1302_Alarm_Write_Time(void) 
{
     
    unsigned char i,tmp;
	//binary number to BCD number
	for(i=0;i<3;i++)
	{                 
		tmp=alarm_buf1[i]/10;
		alarm_buf[i]=alarm_buf1[i]%10;
		alarm_buf[i]=alarm_buf[i]+tmp*16;
	}
	Ds1302_Write_Byte(ds1302_control_add,0x00);			//close write controll protect-> set to 0
	
	Ds1302_Write_Byte(ds1302_alarm_hr_add,alarm_buf[0]);		//hour
	Ds1302_Write_Byte(ds1302_alarm_min_add,alarm_buf[1]);		//min
	Ds1302_Write_Byte(ds1302_alarm_sec_add,alarm_buf[2]);		//sec
	
	Ds1302_Write_Byte(ds1302_control_add,0x80);			//reopen write controll protect
}
/*------------------------------------------------
           Write Alarm time to DS1302
------------------------------------------------*/
void Ds1302_Alarm_Read_Time(void)  
{ 
   	unsigned char i,tmp;
	
	alarm_buf[0]=Ds1302_Read_Byte(ds1302_alarm_hr_add);		//hour
	alarm_buf[1]=Ds1302_Read_Byte(ds1302_alarm_min_add);		//minute
	alarm_buf[2]=Ds1302_Read_Byte(ds1302_alarm_sec_add);//sec
														//sec's msb is represent the counting or not 
	//BCD translate to demical
	for(i=0;i<3;i++)
	{           
		tmp=alarm_buf[i]/16;//ex get 21's 2
		alarm_buf1[i]=alarm_buf[i]%16;//ex get 21's 1
		alarm_buf1[i]=alarm_buf1[i]+tmp*10;
		//alarm[0]->hour
		//alarm[1]->min
		//alarm[2]->sec
	}
	
}
/*------------------------------------------------
                DS1302 Init
------------------------------------------------*/
void Ds1302_Init(void)
{
	
	RST_CLR;			//RST=0
	SCK_CLR;			//SCK=0		 
}


