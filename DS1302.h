
#ifndef __DS1302_H__
#define __DS1302_H__

#include <reg52.h>
#include<intrins.h>

sbit SCK=P1^4;		
sbit SDA=P1^5;		
sbit RST=P1^6;
//��λ��
#define RST_CLR	RST=0//��ƽ�õ�
#define RST_SET	RST=1//��ƽ�ø�


//˫������
#define IO_CLR	SDA=0//��ƽ�õ�
#define IO_SET	SDA=1//��ƽ�ø�
#define IO_R	SDA  //��ƽ��ȡ


//ʱ���ź�
#define SCK_CLR	SCK=0//ʱ���ź�
#define SCK_SET	SCK=1//��ƽ�ø�


#define ds1302_sec_add			0x80		//1000_0000->write sec
#define ds1302_min_add			0x82		//
#define ds1302_hr_add			0x84		//ʱ���ݵ�ַ
#define ds1302_date_add			0x86		//�����ݵ�ַ
#define ds1302_month_add		0x88		//�����ݵ�ַ
#define ds1302_day_add			0x8a		//�������ݵ�ַ
#define ds1302_year_add			0x8c		//�����ݵ�ַ
#define ds1302_control_add		0x8e		//�������ݵ�ַ
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

#define ds1302_alarm_sec_add		0xc0//1100_0000 ram[0].write
#define ds1302_alarm_min_add		0xc2//1100_0010 ram[1].write
#define ds1302_alarm_hr_add		0xc4//1100_0100 ram[2].write


extern unsigned char time_buf2[16];//catch assci on each digital
extern unsigned char time_buf1[8];//��������ʱ������
extern unsigned char time_buf[8] ;//��������ʱ������

extern unsigned char alarm_buf1[3];//demecial
extern unsigned char alarm_buf[3];//BCD
/*------------------------------------------------
           ��DS1302д��һ�ֽ�����
------------------------------------------------*/
void Ds1302_Write_Byte(unsigned char addr, unsigned char d);
/*------------------------------------------------
           ��DS1302����һ�ֽ�����
------------------------------------------------*/
unsigned char Ds1302_Read_Byte(unsigned char addr) ;
/*------------------------------------------------
           ��DS1302д��ʱ������
------------------------------------------------*/
void Ds1302_Write_Time(void) ;
/*------------------------------------------------
           ��DS1302����ʱ������
------------------------------------------------*/
void Ds1302_Read_Time(void)  ;
/*------------------------------------------------
                DS1302��ʼ��
------------------------------------------------*/
void Ds1302_Alarm_Write_Time(void) ;
void Ds1302_Alarm_Read_Time(void)  ;

void Ds1302_Init(void);

#endif
