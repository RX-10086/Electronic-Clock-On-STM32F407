#include "sys.h"
#include "lcd.h"
#include "lcd_init.h"
#include "pic_2.h"

#define TRUE 1
#define FALSE 0

u16 STATE_0 = 1;       //判断暂停状态

char str_output_0[13]="00:00:00:000";
int sec_0 = 0;
int min_0 = 0;
int hour_0 = 0;
int micro_sec_0 = 0;

void TIME_COUNT_FRESH(void)    //TIM4中断事件
{
	  if(micro_sec_0 < 0)
	  {
		  micro_sec_0 = 999;
		  sec_0--;
	  }
		if(sec_0 < 0)
		{
			sec_0 = 59;
			min_0--;
	  }
		if(min_0 < 0)
		{
			min_0 = 59;
			hour_0--;
		}
		if(micro_sec_0 == 0 && sec_0 == 0 && min_0 == 0 && hour_0 == 0)    //倒计时到零时，TIM4停止工作
	  {
		  TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
		  STATE_0 = TRUE;
			return;
	  }
		micro_sec_0--;
}
void TIME_RESET_0(void)      //复位或提前结束倒计时
{
		micro_sec_0 = 0;
	  sec_0 = 0;
	  min_0 = 0;
	  hour_0 = 0;
}
void TIME_STOP_0(void)       //倒计时暂停
{
	if(STATE_0 == FALSE)
	{
	  TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);     
		STATE_0 = TRUE;
	}
	else if(STATE_0 == TRUE)
	{
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
		STATE_0 = FALSE;
	}
}
int OUTPUT_STOP(void)        
{
	return STATE_0;
}
int FRS_STOP(void)
{
	if(micro_sec_0 == 0 && sec_0 == 0 && min_0 == 0 && hour_0 == 0)
		return FALSE;
	else
		return TRUE;
}
void OUTPUT_0(u32 num, char *str)
{
	u32 i = num / 10;
  u32 j = num % 10;
  *str = (char)(i + 48);
  *(str + 1) = (char)(j + 48);
}
void OUTPUT_ms_C(char *str)
{
	u32 i = micro_sec_0 / 100;
	u32 j = (micro_sec_0 % 100) / 10;
	u32 k = micro_sec_0 % 10;
	*str = (char)(i + 48);
	*(str + 1) = (char)(j + 48);
	*(str + 2) = (char)(k + 48);
}
void Display_MODE_3()
{
	LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,GRAYBLUE);
	LCD_ShowString(3,10,"COUNTING",RED,GRAYBLUE,16,0);
	LCD_ShowPicture(10,110,40,40,gImage_OurEDA_Logo_2);
	LCD_ShowString(55,110,"Inspired By",BLACK,GRAYBLUE,12,0);
	LCD_ShowString(55,126,"OurEDA Team",BLACK,GRAYBLUE,12,0);
}
void Display_MODE_3_FRESH()
{
	OUTPUT_0(sec_0, str_output_0 + 6);
	OUTPUT_0(min_0, str_output_0	+ 3);
	OUTPUT_0(hour_0, str_output_0);
	OUTPUT_ms_C(str_output_0 + 9);
	LCD_ShowString(18,30,str_output_0,WHITE,GRAYBLUE,16,0);
}
int ADJ_JUDGE(void)
{
	if(OUTPUT_STOP() == TRUE)
		return 1;
	else
		return 0;
}
void COUNT_ADJ(int adj_flag)
{
	if(adj_flag == 1)
	{
		sec_0++;
	  if(sec_0 == 60) {sec_0 = 0;}
  }
	else if(adj_flag == 2)
	{
		min_0++;
	  if(min_0 == 0) {min_0 = 0;}
	}
	else if(adj_flag == 3)
	{
	  hour_0++;
	  if(hour_0 == 100) {hour_0 = 0;}
  }
}

