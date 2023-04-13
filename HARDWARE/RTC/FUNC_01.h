#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "lcd.h"
#include "lcd_init.h"
#include "Reimu.h"
#include "Marisa.h"

RTC_TimeTypeDef RTC_TimeStruct;     //RTCʱ��
RTC_DateTypeDef RTC_DateStruct;     //RTC����

RTC_TimeTypeDef time_data;    //�洢ʱ��
RTC_DateTypeDef date_data;    //�洢����

u8 tbuf_1[20];       //24H��ʾģʽ
u8 tbuf_2[20];       //12H��ʾģʽ

char WEEK[7][4] = {"MON","TUE","WEN","THU","FRI","SAT","SUN"};      //������ڣ���Ӣ����д��ʽ��

int Week_Day[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
int Week_Day_2[12] = {31,29,31,30,31,30,31,31,30,31,30,31};

void DISPLAY_MODE()   //��������ʾ
{
	LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,GRAYBLUE);
	LCD_ShowPicture(0,36,128,128,gImage_Reimu);     //��ʾ���ǹܡ�����Ƭ
}
void DISPLAY_MODE__FRESH()     //ʱ�䡢������ʾ
{
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf((char*)tbuf_1,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
	LCD_ShowString(10,1,tbuf_1,WHITE,GRAYBLUE,16,0);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)tbuf_2,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
	LCD_ShowString(10,20,tbuf_2,WHITE,GRAYBLUE,12,0); 
	LCD_ShowString(80,20,WEEK[RTC_DateStruct.RTC_WeekDay - 1],YELLOW,GRAYBLUE,12,0);
}
void flag_CHG_DISPLAY()        //�л���12Hģʽʱ����ʾAM/PM��־
{
	LCD_DrawRectangle(96,1,116,21,RED);
	LCD_ShowString(98,2,(RTC_TimeStruct.RTC_Hours <= 12)?"AM":"PM",RED,GRAYBLUE,16,0);
	LCD_ShowPicture(0,32,128,128,gImage_Marisa);     //ħ��ɳ͵������Ҫ��12H��doge��
}
void flag_CHG_DISPLAY_2()      //�л���24Hģʽ
{
	LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,GRAYBLUE);
	LCD_ShowPicture(0,32,128,128,gImage_Reimu);
}
void DISPLAY_MODE__FRESH_12()  //ʱ�䡢������ʾ��ʱ����12Hģʽ��ʾ
{
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	if(RTC_TimeStruct.RTC_Hours > 12)
	{
		sprintf((char*)tbuf_1,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours - 12,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
	}
	else
	{
		sprintf((char*)tbuf_1,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
	}
	LCD_ShowString(10,1,tbuf_1,WHITE,GRAYBLUE,16,0);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)tbuf_2,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
	LCD_ShowString(10,20,tbuf_2,WHITE,GRAYBLUE,12,0); 
	LCD_ShowString(80,20,WEEK[RTC_DateStruct.RTC_WeekDay - 1],YELLOW,GRAYBLUE,12,0);
}
void REAL_TIME_CHG(int rtc_flag_adj)   //�޸ĵ�ǰʱ��
{
	time_data = RTC_TimeStruct;
	switch(rtc_flag_adj)
	{
		case 3:
		{
			time_data.RTC_Hours++;
			if(time_data.RTC_Hours == 24){time_data.RTC_Hours = 0;}
			break;
		}
		case 2:
		{
			time_data.RTC_Minutes++;
			if(time_data.RTC_Minutes == 60){time_data.RTC_Minutes = 0;}
			break;
		}
		case 1:
		{
			time_data.RTC_Seconds = 0;
			break;
		}
	}
	RTC_Set_Time(time_data.RTC_Hours,time_data.RTC_Minutes,time_data.RTC_Seconds,RTC_H12_AM);
}
void REAL_DATE_CHG(int rtc_flag_adj)
{
	date_data = RTC_DateStruct;
	switch(rtc_flag_adj)
	{
		case 4:
		{
			date_data.RTC_Year++;
			if(date_data.RTC_Year == 99){date_data.RTC_Year = 0;}
			break;
		}
		case 5:
		{
			date_data.RTC_Month++;
			if(date_data.RTC_Month == 13){date_data.RTC_Month = 1;}
			break;
		}
		case 6:
		{
			date_data.RTC_Date++;
			if(date_data.RTC_Year / 4 == 0)
			{
        if(date_data.RTC_Date == Week_Day_2[date_data.RTC_Month]){date_data.RTC_Date = 1;}
		  }
			else
			{
				if(date_data.RTC_Date == Week_Day[date_data.RTC_Month]){date_data.RTC_Date = 1;}
			}
			break;
		}
		case 7:
		{
			date_data.RTC_WeekDay++;
			if(date_data.RTC_WeekDay == 8){date_data.RTC_WeekDay = 1;}
			break;
		}
	}
	RTC_Set_Date(date_data.RTC_Year,date_data.RTC_Month,date_data.RTC_Date,date_data.RTC_WeekDay);
}
void USART_REC_TIME()     //ͨ�������޸ĵ�ǰʱ�䡢����
{
	  u16 usart_len;
	  int i, j, k, l, m, n;
    const char *JD1 = "TIME:";
	  //const char *JD2 = "DATE:";
    u16 ch = 0;
	  time_data = RTC_TimeStruct;
	  ch = 0;
	  if(USART_RX_STA&0x8000)
		{
			u16 t = 0;
			for(t = 0;t < 4;t++)
			{
				if(*(USART_RX_BUF + t) != *(JD1 + t))
				{
					printf("\r\nERROR!\r\n");
					return;
				}
			}		
			usart_len = USART_RX_STA&0x3fff;  			//�õ��˴ν��յ������ݳ���
		  i = (int)(*(USART_RX_BUF + 5)) - 48, j = (int)(*(USART_RX_BUF + 6)) - 48;
      time_data.RTC_Hours = i * 10 + j;
      k = (int)(*(USART_RX_BUF + 8)) - 48, l = (int)(*(USART_RX_BUF + 9)) - 48;
      time_data.RTC_Minutes = k * 10 + l;
      m = (int)(*(USART_RX_BUF + 11)) - 48, n = (int)(*(USART_RX_BUF + 12)) - 48;
      time_data.RTC_Seconds = m * 10 + n;
			RTC_Set_Time(time_data.RTC_Hours,time_data.RTC_Minutes,time_data.RTC_Seconds,RTC_H12_AM);
    }
			USART_RX_STA = 0;
			delay_ms(100);
}
