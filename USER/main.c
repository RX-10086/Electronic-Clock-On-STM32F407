#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "rtc_FUNC_01.h"
#include "timer_3_FUNC_02.h"
#include "timer_4_FUNC_03.h"
#include "key.h"
#include "lcd.h"
#include "lcd_init.h"
#include "Cirno.h"    //Baka----Baka! (Idiot, idiot!)
#include "rtc.h"

#define TRUE 1
#define FALSE 0

#define MODE_1 1
#define MODE_2 2
#define MODE_3 3

u16 timer_first_start_state = FALSE;     //判断是否第一次进入秒表模式标识
u16 Count_first_start_state = FALSE;     //判断是否第一次进入倒计时模式标识

int MODE = MODE_1;       //记录模式，开机时，默认模式为主界面模式
int FRESH_STATE = 0;     //判断由某一模式进入另一模式时，是否刷新屏幕

u16 key = -1;            //记录按下哪个按键

void Display(void);      //控制显示模式
void MODE_CHG(void);     //改变模式变量

void flag_CHG(void);     //主界面模式中，切换12H/24H模式
int flag_FRESH = FALSE;    //切换12H/24H模式时，判断是否需要刷新屏幕

int flag_H12_24 = FALSE;   //记录12H/24H模式，默认为24H模式

int flag_md1 = 0;          //主模式中，存储对应要修改的变量为时、分、秒，同时对应屏幕光标
int flag_md3 = 0;          //倒计时模式中，存储对应要设定的变量为时、分、秒，同时对应屏幕光标

const int length = 24;     //光标长度
const int length_s = 16;

int USART_Re = FALSE;

void USART_START(void);

int main(void)             //主函数
{ 
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	    
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);     
	uart_init(115200);	
	LED_Init();					
	KEY_Init();
	LCD_Init();
	My_RTC_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,GRAYBLUE);
	LCD_ShowPicture(0,0,128,128,gImage_Cirno);       //开机进入主界面前，显示（9）的照片
	LCD_ShowString(0,130,"Baka----Baka!",WHITE,GRAYBLUE,12,0);      //翻译:笨蛋，笨蛋！
	delay_ms(3500);      //延时3.5s，然而并没有什么卵用
	while(TRUE) 
	{	
		key = KEY_Scan(0);   //检测并记录按键状态
		if(key != 0)         //如果有按键输入
		{
			switch(key)
			{
				case KEY0_PRES:   //按下K0
				{
          if(MODE == MODE_1) {flag_CHG();}  
					if(MODE == MODE_2) {TIME_RESET();}
					if(MODE == MODE_3 && ADJ_JUDGE() == TRUE) {TIME_RESET_0();}
				  break;
				}
				case KEY1_PRES:   //按下K1
				{
					if(MODE == MODE_1)
					{
						USART_Re = (USART_Re==FALSE);
						if(USART_Re == TRUE)
						{
							LCD_DrawRectangle(86,1,96,11,YELLOW);
						}
						else
						{
							LCD_DrawRectangle(86,1,96,11,GRAYBLUE);
						}
					}
					if(timer_first_start_state == FALSE && MODE == MODE_2)
					{
						timer_first_start_state = TRUE;
						TIM3_Int_Init(10-1, 8400-1);   //启动定时器3
					}
					if(Count_first_start_state == FALSE && MODE == MODE_3 && FRS_STOP() == TRUE)
					{
						Count_first_start_state = TRUE;
						TIM4_Int_Init(10-1,8400-1);    //启动定时器4
					}
					else
						{
					    if(MODE == MODE_2)  
					    {
					      TIME_STOP();
					    }
					    if(MODE == MODE_3 && FRS_STOP() == TRUE)
					    {
						    TIME_STOP_0();
					    }
					  }
				  break;
				}
				case KEY2_PRES:
				{
					if(MODE == MODE_1)
					{
						if(flag_md1 < 4)
						  REAL_TIME_CHG(flag_md1);
						else
							REAL_DATE_CHG(flag_md1);
					}
					if(MODE == MODE_2)
					{
						record_md2();
					}
					if(MODE == MODE_3)
					{
					  COUNT_ADJ(flag_md3);
					}
					break;
				}
				case KEY3_PRES:
				{
					if(MODE == MODE_1 && flag_H12_24 == FALSE)
					{
						flag_md1++;
						if(flag_md1 <= 4)
						{
						 if(flag_md1 == 4)
						 {
							 LCD_DrawRectangle(10,16,10 + length,18,GRAYBLUE);
						 }
						 else if(flag_md1 >= 2 && flag_md1 < 4)
						 {
							 LCD_DrawRectangle(10 + (4 - flag_md1)*length,16,10 + (5 - flag_md1)*length,18,GRAYBLUE);
						 }
						 if(flag_md1 != 0 && flag_md1 != 4)
						 {
							 LCD_DrawRectangle(10 + (3 - flag_md1)*length,16,10 + (4 - flag_md1)*length,18,GREEN);
						 }
					 }
					 if(flag_md1 <= 8 && flag_md1 >= 4)
						{
							if(flag_md1 == 8)
							{
								LCD_DrawRectangle(20 + 3*length_s,31,20 + 4*length_s,33,GRAYBLUE);
							  flag_md1 = 0;
							  break;
							}
							else if(flag_md1 >= 5 && flag_md1 < 8)
							{
								LCD_DrawRectangle(20 + (flag_md1 - 5)*length_s,31,20 + (flag_md1 - 4)*length_s,33,GRAYBLUE);
							}
							if(flag_md1 != 0)
							{
								LCD_DrawRectangle(20 + (flag_md1 - 4)*length_s,31,20 + (flag_md1 - 3)*length_s,33,GREEN);
							}
						}
					}
					if(MODE == MODE_2 && OUT_STATE() == TRUE)
					{
						record_reset();
						Display_MODE_2();
					}
					if(MODE == MODE_3 && ADJ_JUDGE() == TRUE)
					{
						flag_md3++;
						if(flag_md3 <= 4)
						{
						 if(flag_md3 == 4)
						 {
							 LCD_DrawRectangle(18,50,18 + length,52,GRAYBLUE);
							 flag_md3 = 0;
							 break;
						 }
						 else if(flag_md3 >= 2)
						 {
						   LCD_DrawRectangle(18 + (4 - flag_md3)*length,50,18 + (5 - flag_md3)*length,52,GRAYBLUE);
						 }
						 if(flag_md3 != 0)
						 {
						    LCD_DrawRectangle(18 + (3 - flag_md3)*length,50,18 + (4 - flag_md3)*length,52,GREEN);
						 }
					  }
					}
					break;
				}
				case WKUP_PRES:
				{
					MODE_CHG();
				  break;
				}
			}    
		}
		else
		{
			USART_START();
		}
		Display();
	}
}
void Display()
{
	if(MODE == MODE_1)
	{
		if(FRESH_STATE != MODE_1)
		{
			DISPLAY_MODE();
			FRESH_STATE = MODE_1;
		}
		if(flag_H12_24 == TRUE)
		{
			flag_CHG_DISPLAY();
		  flag_FRESH = TRUE;
		}
		if(flag_H12_24 == FALSE)
		{
			if(flag_FRESH == TRUE) {flag_CHG_DISPLAY_2();}
			flag_FRESH = FALSE;
		}
		if(flag_H12_24 == FALSE) {DISPLAY_MODE__FRESH();}
		if(flag_H12_24 == TRUE) {DISPLAY_MODE__FRESH_12();}
	}
	if(MODE == MODE_2)
	{
		if(FRESH_STATE != MODE_2) 
    {
		  Display_MODE_2();
			FRESH_STATE = MODE_2;
		}
		Display_MODE_2_FRESH();
	}
	if(MODE == MODE_3)
	{
		if(FRESH_STATE != MODE_3) 
    {
		  Display_MODE_3();
			FRESH_STATE = MODE_3;
		}
		Display_MODE_3_FRESH();
	}
}
void MODE_CHG()
{
	if(MODE >= 3)
	{
		MODE = MODE_1;
		return;
	}
	MODE++;
}
void flag_CHG()
{
	if(flag_H12_24 == TRUE) {flag_H12_24 = FALSE;}
	else {flag_H12_24 = TRUE;}
}
void USART_START()
{
	if(USART_Re == TRUE)
	{
		USART_REC_TIME();
	}
}

