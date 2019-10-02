#include<reg52.h> 
#include<intrins.h>

sbit SPK = P1^2;    //定義喇叭端口
sbit RS = P2^4;   //定義端口 
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
unsigned char frq; // 宣告頻率
unsigned char num;
unsigned char Timer0_H,Timer0_L,Time;

void DelayUs2x(unsigned char t);//函數聲明 
void DelayMs(unsigned char t);

code unsigned char FREQH[]={
    0xFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE,
    0XFE, 0XFE, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFF
} ;
                         // 音階頻率表 低八位
code unsigned char FREQL[]={
    0xFA, 0XEB, 0XDA, 0XC9, 0XB6, 0XA2, 0X8E, 0X78, 0X61, 0X48,
    0X2E, 0X12, 0XF5, 0XD6, 0XB5, 0X92, 0X6D, 0XFF
};

void Init_Timer0(void)
{
 TMOD |= 0x21;	  //使用模式1，16位定時器，使用"|"符號可以在使用多個定時器時不受影響		     
 //TH0=0x00;	      //給定初值
 //TL0=0x00;
 EA=1;            //總中斷打開
 ET0=1;           //定時器中斷打開
 TR0=1;           //定時器開關打開
}

void output(unsigned char num){
	Init_Timer0(); //初始化定時器

       
      //DelayMs(1); //延時1ms，累加頻率值
	  frq = num;
          
	
}


void Timer0_isr(void) interrupt 1 
{
 TH0=Timer0_H;		  //重新賦值 
 TL0=Timer0_L;         //低8位值在主程序中不斷累加
 
 SPK=!SPK;        //端口電平取反

}


void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: 模式 1, 8-bit UART, 使能接收  
    TMOD |= 0x21;               // TMOD: timer 1, mode 2, 8-bit 重裝
    TH1   = 0xFD;               // TH1:  重裝值 9600 波特率 晶振 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 打開                         
    EA    = 1;                  //打開總中斷
    ES    = 1;                  //打開串口中斷
}    


void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    發送一個字符串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 表示字符串結束標誌，通過檢測是否字符串末尾
  {
  SendByte(*s);
  s++;
  }
}
/*------------------------------------------------
                     串口中斷程序
------------------------------------------------*/
void UART_SER (void) interrupt 4 //串行中斷服務程序
{
    unsigned char Temp;          //定義臨時變量 
   
   if(RI)                        //判斷是接收中斷產生
     {
	  RI=0;                      //標誌位清零
	  Temp=SBUF;                 //讀入緩衝區的值
	  recbuf[head] = Temp;                  

      head++;
      if (head == Limit)
          head = 0;                 
	 }
 //  if(TI)                        //如果是發送標誌位，清零
 //    TI=0;
} 

/*------------------------------------------------
 uS延時函數，含有輸入參數 unsigned char t，無返回值
 unsigned char 是定義無符號字符變量，其值的範圍是
 0~255 這裡使用晶振12M，精確延時請使用彙編,大致延時
 長度如下 T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS延時函數，含有輸入參數 unsigned char t，無返回值
 unsigned char 是定義無符號字符變量，其值的範圍是
 0~255 這裡使用晶振12M，精確延時請使用彙編
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //大致延時1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*------------------------------------------------
              判忙函數
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
              寫入命令函數
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
// while(LCD_Check_Busy()); //忙則等待
 DelayMs(5);
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              寫入數據函數
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 //while(LCD_Check_Busy()); //忙則等待
 DelayMs(5);
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }

/*------------------------------------------------
                清屏函數
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              寫入字符串函數
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
              寫入字符函數
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
              初始化函數
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*顯示模式設置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*顯示關閉*/ 
   LCD_Write_Com(0x01);    /*顯示清屏*/ 
   LCD_Write_Com(0x06);    /*顯示光標移動設置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*顯示開及光標設置*/
   }
   
   
void Song()
{
 TH0=Timer0_H;//賦值定時器時間，決定頻率
 TL0=Timer0_L;
 TR0=1;       //打開定時器
 //延時所需要的節拍                      
 }
/*------------------------------------------------
                    主函數
------------------------------------------------*/ 
void main(void) 
{ 
unsigned char i; 
TMOD|=0x21; //置定時器0工作方式1
EA=1;       //打開全局中斷
ET0=1;      //打開定時0中斷
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

