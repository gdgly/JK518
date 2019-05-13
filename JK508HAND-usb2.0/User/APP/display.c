/**
  ******************************************************************************
  * @file    sysytem.c
  * @author  fire
  * @version V1.0
  * @date    2018-04-29
  * @brief   �ֳֶ�·�¶Ȳ�����ϵͳ���ý���
 
  ******************************************************************************
  */
  
#include "./led/bsp_led.h"
#include "./lcd/bsp_lcd.h"
#include "./key/bsp_key.h"
#include "./beep/bsp_beep.h"
#include "./tim/bsp_basic_tim.h"
#include "jk508.h"

extern union 
{
   unsigned char FLAG_VAL7;
   struct 
   {
       unsigned char  FN:2;    //����
       unsigned char  SP:2;    //�ٶ�
       unsigned char  BD:3;    //����
       unsigned char  LG:1;	   //����
   }BIT_FLAG;
}FLAG7;

extern union 
{
   unsigned char FLAG_VAL8;
   struct 
   {
       unsigned char  BR:3;    //����
       unsigned char  DM:3;    //��������
       
   }BIT_FLAG;
}FLAG8;

void page_setdis(void)
{
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/
	
//	char buf[10];
	
	DrawMenu();
	Drawset_dismenu();
	if(LANG == chs)
	{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "������ʾ");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(54,10,"�¶Ȳ���");
		LCD_DisplayStringLine(94,10,"�¶�ϵ��");
//		LCD_DisplayStringLine(134,10,"�˺�");
		LCD_DisplayStringLine(134,10,"�ο��¶�");
		LCD_DisplayStringLine(174,10,"������ʱ");
		LCD_DisplayStringLine(214,10,"����");
		LCD_DisplayStringLine(254,10,"�缫����");


	}else{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(3,10, "<SETUP DISPLAY>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,13,"REL");
		LCD_DisplayStringLine(90,10,"TC-PAR");
//		LCD_DisplayStringLine(130,10,"ACCOUNT");
		LCD_DisplayStringLine(130,10,"TC-TEP");
		LCD_DisplayStringLine(170,10,"RANGE DELAY");
		LCD_DisplayStringLine(210,10,"CALCULATION");
		LCD_DisplayStringLine(250,10,"CONSTANT");


	}
	
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	if(LANG == chs)
	{
	

		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_SetBackColor(LCD_COLOR_BACK);
			if(REL == op_on)
		{
			LCD_DisplayStringLine(54,210,"��");
		}else{
			LCD_DisplayStringLine(54,210,"�ر�");
		}

		if(CALCULATION == op_on)
		{
			LCD_DisplayStringLine(214,210,"��");
		}else{
			LCD_DisplayStringLine(214,210,"�ر�");
		}
	}else{
	
	
		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_SetBackColor(LCD_COLOR_BACK);
		if(REL == op_on)
		{
			LCD_DisplayStringLine(50,210,"ON");
		}else{
			LCD_DisplayStringLine(50,210,"OFF");
		}
		if(CALCULATION == op_on)
		{
			LCD_DisplayStringLine(210,210,"ON");
		}else{
			LCD_DisplayStringLine(210,210,"OFF");
		}
			
	}
	RangeDelay=100; 		
	delaytimefocus_on(RangeDelay);
  		
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	LCD_DisplayStringLine(90,360,"%");
	DISP_CNL_S(127,344,"o");
    LCD_DisplayStringLine(130,360,"C");
	 TC_PAR = 100.675;
    tc_parfocus_off(TC_PAR);
	TC_TEP= 99.675;
		tc_tepfocus_off(TC_TEP);
	CONSTANT= 199.6751;
		consfocus_off(CONSTANT);
	LCD_DisplayStringLine(170,360,"mS");
	if(LANG == chs)
	{
		DrawInstruction("�޸���ʱ");
	}else{
		DrawInstruction("Input Range Delay");
	}
	page_flag = setdis;
	op_flag = set_delay;
}
