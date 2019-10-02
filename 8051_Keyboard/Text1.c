#include<reg52.h> 
#include<intrins.h>

sbit SPK = P1^2;    //�w�q��z�ݤf
sbit RS = P2^4;   //�w�q�ݤf 
sbit RW = P2^5;
sbit EN = P2^6;

#define RS_CLR RS=0 
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1

#define DataPort P0
#define Limit    16

sbit LED0 = P1^0;
sbit LED1 = P1^1;
sbit LED2 = P1^2;
sbit LED3 = P1^3;
sbit LED4 = P1^4;
sbit LED5 = P1^5;
sbit LED6 = P1^6;
sbit LED7 = P1^7;
#define ON 0
#define OFF 1


unsigned char recbuf[Limit];
unsigned char head = 0;
unsigned char tail = 0;
unsigned char frq; // �ŧi�W�v
unsigned char num;
unsigned char Timer0_H,Timer0_L,Time;

void DelayUs2x(unsigned char t);//����n�� 
void DelayMs(unsigned char t);

code unsigned char FREQH[]={
    0xFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE,
    0XFE, 0XFE, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFF
} ;
                         // �����W�v�� �C�K��
code unsigned char FREQL[]={
    0xFA, 0XEB, 0XDA, 0XC9, 0XB6, 0XA2, 0X8E, 0X78, 0X61, 0X48,
    0X2E, 0X12, 0XF5, 0XD6, 0XB5, 0X92, 0X6D, 0XFF
};

void Init_Timer0(void)
{
 TMOD |= 0x21;	  //�ϥμҦ�1�A16��w�ɾ��A�ϥ�"|"�Ÿ��i�H�b�ϥΦh�өw�ɾ��ɤ����v�T		     
 //TH0=0x00;	      //���w���
 //TL0=0x00;
 EA=1;            //�`���_���}
 ET0=1;           //�w�ɾ����_���}
 TR0=1;           //�w�ɾ��}�����}
}

void output(unsigned char num){
	Init_Timer0(); //��l�Ʃw�ɾ�

       
      //DelayMs(1); //����1ms�A�֥[�W�v��
	  frq = num;
          
	
}


void Timer0_isr(void) interrupt 1 
{
 TH0=Timer0_H;		  //���s��� 
 TL0=Timer0_L;         //�C8��Ȧb�D�{�Ǥ����_�֥[
 
 SPK=!SPK;        //�ݤf�q������

}


void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: �Ҧ� 1, 8-bit UART, �ϯ౵��  
    TMOD |= 0x21;               // TMOD: timer 1, mode 2, 8-bit ����
    TH1   = 0xFD;               // TH1:  ���˭� 9600 �i�S�v ���� 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 ���}                         
    EA    = 1;                  //���}�`���_
    ES    = 1;                  //���}��f���_
}    


void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    �o�e�@�Ӧr�Ŧ�
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 ��ܦr�Ŧ굲���лx�A�q�L�˴��O�_�r�Ŧ꥽��
  {
  SendByte(*s);
  s++;
  }
}
/*------------------------------------------------
                     ��f���_�{��
------------------------------------------------*/
void UART_SER (void) interrupt 4 //��椤�_�A�ȵ{��
{
    unsigned char Temp;          //�w�q�{���ܶq 
   
   if(RI)                        //�P�_�O�������_����
     {
	  RI=0;                      //�лx��M�s
	  Temp=SBUF;                 //Ū�J�w�İϪ���
	  recbuf[head] = Temp;                  

      head++;
      if (head == Limit)
          head = 0;                 
	 }
 //  if(TI)                        //�p�G�O�o�e�лx��A�M�s
 //    TI=0;
} 

/*------------------------------------------------
 uS���ɨ�ơA�t����J�Ѽ� unsigned char t�A�L��^��
 unsigned char �O�w�q�L�Ÿ��r���ܶq�A��Ȫ��d��O
 0~255 �o�̨ϥδ���12M�A��T���ɽШϥηJ�s,�j�P����
 ���צp�U T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS���ɨ�ơA�t����J�Ѽ� unsigned char t�A�L��^��
 unsigned char �O�w�q�L�Ÿ��r���ܶq�A��Ȫ��d��O
 0~255 �o�̨ϥδ���12M�A��T���ɽШϥηJ�s
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //�j�P����1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*------------------------------------------------
              �P�����
------------------------------------------------*/
 bit LCD_Check_Busy(void) 
 { 
 DataPort= 0xFF; 
 RS_CLR; 
 RW_SET; 
 EN_CLR; 
 _nop_(); 
 EN_SET;
 return (bit)(DataPort & 0x80);
 }
/*------------------------------------------------
              �g�J�R�O���
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
// while(LCD_Check_Busy()); //���h����
 DelayMs(5);
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              �g�J�ƾڨ��
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 //while(LCD_Check_Busy()); //���h����
 DelayMs(5);
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }

/*------------------------------------------------
                �M�̨��
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              �g�J�r�Ŧ���
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     
 	}
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              �g�J�r�Ũ��
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 LCD_Write_Data( Data);  
 }
/*------------------------------------------------
              ��l�ƨ��
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*��ܼҦ��]�m*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*�������*/ 
   LCD_Write_Com(0x01);    /*��ܲM��*/ 
   LCD_Write_Com(0x06);    /*��ܥ��в��ʳ]�m*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*��ܶ}�Υ��г]�m*/
   }
   
   
void Song()
{
 TH0=Timer0_H;//��ȩw�ɾ��ɶ��A�M�w�W�v
 TL0=Timer0_L;
 TR0=1;       //���}�w�ɾ�
 //���ɩһݭn���`��                      
 }
/*------------------------------------------------
                    �D���
------------------------------------------------*/ 
void main(void) 
{ 
unsigned char i; 
TMOD|=0x21; //�m�w�ɾ�0�u�@�覡1
EA=1;       //���}�������_
ET0=1;      //���}�w��0���_
SPK=0;

InitUART();

i=0;
	while (1) 
	 {  
	
	      if (head == 1)
	      {
              head = 0;
              switch(recbuf[0])
              {

                    case 1:	
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 2: 
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 3:	
							TR0=1;	
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 4:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 5:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 6:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 7:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
							
							
                    case 8:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 9:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 10:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 11:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 12:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;
                    case 13:
							TR0=1;
							Timer0_H=FREQH[recbuf[0]-1];
							Timer0_L=FREQL[recbuf[0]-1];
							//Song();
                            break;	
/*---------------------------------------------------------------------------------------------------------*/				               
                    case 14:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 15:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 16:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 17:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 18:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 19:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 20:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 21:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 22:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 23:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 24:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 25:
							TR0 = 0;
							SPK = 0;
                            break;
                    case 26:
							TR0 = 0;
							SPK = 0;
                            break;
               }
			   SPK = 0;

	     }
	}

}

