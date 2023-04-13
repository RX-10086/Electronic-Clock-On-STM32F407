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

u16 timer_first_start_state = FALSE;     //�ж��Ƿ��һ�ν������ģʽ��ʶ
u16 Count_first_start_state = FALSE;     //�ж��Ƿ��һ�ν��뵹��ʱģʽ��ʶ

int MODE = MODE_1;       //��¼ģʽ������ʱ��Ĭ��ģʽΪ������ģʽ
int FRESH_STATE = 0;     //�ж���ĳһģʽ������һģʽʱ���Ƿ�ˢ����Ļ

u16 key = -1;            //��¼�����ĸ�����

void Display(void);      //������ʾģʽ
void MODE_CHG(void);     //�ı�ģʽ����

void flag_CHG(void);     //������ģʽ�У��л�12H/24Hģʽ
int flag_FRESH = FALSE;    //�л�12H/24Hģʽʱ���ж��Ƿ���Ҫˢ����Ļ

int flag_H12_24 = FALSE;   //��¼12H/24Hģʽ��Ĭ��Ϊ24Hģʽ

int flag_md1 = 0;          //��ģʽ�У��洢��ӦҪ�޸ĵı���Ϊʱ���֡��룬ͬʱ��Ӧ��Ļ���
int flag_md3 = 0;          //����ʱģʽ�У��洢��ӦҪ�趨�ı���Ϊʱ���֡��룬ͬʱ��Ӧ��Ļ���

const int length = 24;     //��곤��
const int length_s = 16;

int USART_Re = FALSE;

void USART_START(void);

int main(void)             //������
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
	LCD_ShowPicture(0,0,128,128,gImage_Cirno);       //��������������ǰ����ʾ��9������Ƭ
	LCD_ShowString(0,130,"Baka----Baka!",WHITE,GRAYBLUE,12,0);      //����:������������
	delay_ms(3500);      //��ʱ3.5s��Ȼ����û��ʲô����
	while(TRUE) 
	{	
		key = KEY_Scan(0);   //��Ⲣ��¼����״̬
		if(key != 0)         //����а�������
		{
			switch(key)
			{
				case KEY0_PRES:   //����K0
				{
          if(MODE == MODE_1) {flag_CHG();}  
					if(MODE == MODE_2) {TIME_RESET();}
					if(MODE == MODE_3 && ADJ_JUDGE() == TRUE) {TIME_RESET_0();}
				  break;
				}
				case KEY1_PRES:   //����K1
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
						TIM3_Int_Init(10-1, 8400-1);   //������ʱ��3
					}
					if(Count_first_start_state == FALSE && MODE == MODE_3 && FRS_STOP() == TRUE)
					{
						Count_first_start_state = TRUE;
						TIM4_Int_Init(10-1,8400-1);    //������ʱ��4
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

