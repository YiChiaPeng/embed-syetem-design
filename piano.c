#include<reg52.h>
#define KeyPort  P2     //lower keyboard
#define KeyPort1 P0     //matrix keyboard
sbit SPK=P1^0;          //speaker pin
unsigned char High,Low; //buffer of TH0 and TL0

//note frequency
unsigned char code freq[][2]={

  0x42,0xF2,//00440HZ 1   
  0xC1,0xF3,//00494HZ 2  
  0x17,0xF5,//00554HZ 3  
  0xB6,0xF5,//00587HZ 4    
  0x68,0xF6,//00659HZ 5     
  0xE9,0xF7,//00740HZ 6     
  0x5B,0xF8,//00831HZ 7

  0x21,0xF9,//00440HZ 8   
  0xE1,0xF9,//00494HZ 9  
  0x8C,0xFA,//00554HZ 10  
  0xD8,0xFA,//00587HZ 11    
  0x68,0xFB,//00659HZ 12     
  0xE9,0xFB,//00740HZ 13     
  0x5B,0xFC,//00831HZ 14

  0x8F,0xFC,//00440HZ 15   
  0xEE,0xFC,//00494HZ 16  
  0x44,0xFD,//00554HZ 17  
  0x6B,0xFD,//00587HZ 18    
  0xB4,0xFD,//00659HZ 19     
  0xF4,0xFD,//00740HZ 20     
  0x2D,0xFE,//00831HZ 21

  0x47,0xFE,//00440HZ 22   
  0x77,0xFE,//00494HZ 23  
  0xA2,0xFE,//00554HZ 24  
  0xB6,0xFE,//00587HZ 25    
  0xDA,0xFE,//00659HZ 26     
  0xFA,0xFE,//00740HZ 27     
  0x16,0xFF,//00831HZ 28
}; 
void delay(unsigned int d)
{
unsigned int i,j;
	for(i=0;i<10;i++)
		for(j=0;j<d;j++);
}
//which key is pressed
//higher 4 bit is keyboard input
unsigned char KeyScan(void)  
{
	unsigned char Val;
	KeyPort1=0xf0; //P1 
	if(KeyPort1!=0xf0)//if you press any key 
 	{
		delay(10);//debounce check  
		if(KeyPort1!=0xf0)//if you really press any key 
	  	{           
      		KeyPort1=0xfe;//check row 1 
		  	if(KeyPort1!=0xfe)
	    	{
				Val=KeyPort1&0xf0;//set last 4 bits to the zero
				Val+=0x0e;//1110
        		//key pressed with continuous sound
	  	  		//while(KeyPort1!=0xfe);
			  	//delay(10); 
			  	//while(KeyPort1!=0xfe);
	      		return Val;
			}
      		KeyPort1=0xfd;//check row 2;1101->13->d 
		  	if(KeyPort1!=0xfd)
			{
				Val=KeyPort1&0xf0;
	  	  		Val+=0x0d;//1101
	  	  		//while(KeyPort1!=0xfd);
			  	//delay(10); 
			  	//while(KeyPort1!=0xfd);
	      		return Val;
			}
			KeyPort1=0xfb;//check row 3;1011->11->b 
		  	if(KeyPort1!=0xfb)
			{
		    	Val=KeyPort1&0xf0;
	  	  		Val+=0x0b;//1011
	  	  		//while(KeyPort1!=0xfb);
			  	//delay(10); 
			  	//while(KeyPort1!=0xfb);
	      		return Val;
			} 
      		KeyPort1=0xf7;//check row 4;0111->7->7
		  	if(KeyPort1!=0xf7)
	    	{
				Val=KeyPort1&0xf0;
	  	  		Val+=0x07;//0111
	  			//while(KeyPort1!=0xf7);
			  	//delay(10); 
			  	//while(KeyPort1!=0xf7);
	     		return Val;
	    	}
		}
	}
	return 0xff;
}

//translated the hex button signal to button id
unsigned char KeyPro(void)
{
 switch(KeyScan())
 	{
  		case 0x7e:return 0;break;//0 
  		case 0x7d:return 1;break;//1
  		case 0x7b:return 2;break;//2
  		case 0x77:return 3;break;//3
  		case 0xbe:return 4;break;//4
  		case 0xbd:return 5;break;//5
  		case 0xbb:return 6;break;//6
 	 	case 0xb7:return 7;break;//7
  		case 0xde:return 8;break;//8
  		case 0xdd:return 9;break;//9
  		case 0xdb:return 10;break;//a
  		case 0xd7:return 11;break;//b
  		case 0xee:return 12;break;//c
  		case 0xed:return 13;break;//d
  		case 0xeb:return 14;break;//e
  		case 0xe7:return 15;break;//f
  		default:return 0xff;break;
 	}
 }
 /*------------------------------------------------
                  嚙踝蕭嚙踝蕭嚙踝蕭嚙踝蕭
------------------------------------------------*/
void Init_Timer0(void);//嚙踝蕭奀嚙踝蕭嚙踝蕭宎嚙踝蕭
/*------------------------------------------------
                    嚙踝蕭嚙踝蕭嚙踝蕭
------------------------------------------------*/
void main (void)
{
	unsigned char num;                  

	Init_Timer0();    //嚙踝蕭宎嚙踝蕭嚙踝蕭奀嚙踝蕭0嚙踝蕭嚙踝蕭猁嚙踝蕭嚙踝蕭嚙踝蕭嚙踝蕭僆嚙諒界剁蕭嚙?
	SPK=0;            //嚙踝蕭帤嚙踝蕭嚙踝蕭奀嚙踝蕭嚙踝蕭嚙褓腴蛛蕭嚙踝蕭嚙踝蕭砦嚙踝蕭嚙誹詢蛛蕭嚙踝蕭嚙踝蕭嚙踝蕭
	while (1)         //嚙踝蕭悜嚙踝蕭
	{
   	
	
    	//lower keyboard
		//decide which keyboard did you press
		if(KeyPort!=0xff)
		{
			switch(KeyPort)
			{
		 		case 0x7f:num= 22;break;
		 		case 0xdf:num= 23;break;
		 		case 0xf7:num= 24;break;
		 		case 0xfd:num= 25;break;
		 		case 0xbf:num= 26;break;
		 		case 0xef:num= 27;break;
		 		case 0xfb:num= 28;break;
		 		//case 0xfe:num= 8;break;
		 		default:num= 0;break;
			}
		}else{
			//matrix keyboard
			switch(KeyPro())
			{
		  		case 0:num= 9;break;
		  		case 1:num= 10;break;
		  		case 2:num= 11;break;
		  		case 3:num= 12;break;
		  		case 4:num= 13;break;
		  		case 5:num= 14;break;
		  		case 6:num= 15;break;

				case 8:num= 16;break;
      			case 9:num= 17;break;
		  		case 10:num= 18;break;
		  		case 11:num= 19;break;
		  		case 12:num= 20;break;
		  		case 13:num= 21;break;
				case 14:num= 22;break;
				case 15:num= 23;break;
		  		default:num=0;break;
			}
		}
    	if(num==0)
    	{
      		TR0=0;    //timer0 end up counter
      		SPK=0;    //turn off speaker
	  	}
    	else 
    	{
	    	High=freq[num-1][1];
   			Low =freq[num-1][0];
		  	TR0=1;    //timer start counter
	  	}
	}
}


/*------------------------------------------------
          Timer0 initial process
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //妏嚙踝蕭耀宒1嚙踝蕭16弇嚙踝蕭奀嚙踝蕭嚙踝蕭妏嚙踝蕭"|"嚙踝蕭嚙褐選蕭嚙踝蕭嚙踝蕭妏嚙衛塚蕭嚙踝蕭嚙褊梧蕭嚙褊梧蕭嚙踝蕭嚙諉堆蕭嚙?	     
 EA=1;            //嚙踝蕭嚙請剿湛蕭
 ET0=1;           //嚙踝蕭奀嚙踝蕭嚙請剿湛蕭
 //TR0=1;           //嚙踝蕭奀嚙踝蕭嚙踝蕭嚙諍湛蕭
}
/*------------------------------------------------
          Timer0 interupt service program
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
 TH0=High;
 TL0=Low;
 SPK=!SPK;
 
}

