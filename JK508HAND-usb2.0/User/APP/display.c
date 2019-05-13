/**
  ******************************************************************************
  * @file    sysytem.c
  * @author  fire
  * @version V1.0
  * @date    2018-04-29
  * @brief   手持多路温度测试仪系统配置界面
 
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
       unsigned char  FN:2;    //字体
       unsigned char  SP:2;    //速度
       unsigned char  BD:3;    //波特
       unsigned char  LG:1;	   //语言
   }BIT_FLAG;
}FLAG7;

extern union 
{
   unsigned char FLAG_VAL8;
   struct 
   {
       unsigned char  BR:3;    //亮度
       unsigned char  DM:3;    //降低亮度
       
   }BIT_FLAG;
}FLAG8;

void page_setdis(void)
{
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	
//	char buf[10];
	
	DrawMenu();
	Drawset_dismenu();
	if(LANG == chs)
	{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "设置显示");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(54,10,"温度补偿");
		LCD_DisplayStringLine(94,10,"温度系数");
//		LCD_DisplayStringLine(134,10,"账号");
		LCD_DisplayStringLine(134,10,"参考温度");
		LCD_DisplayStringLine(174,10,"量程延时");
		LCD_DisplayStringLine(214,10,"计算");
		LCD_DisplayStringLine(254,10,"电极常数");


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
			LCD_DisplayStringLine(54,210,"打开");
		}else{
			LCD_DisplayStringLine(54,210,"关闭");
		}

		if(CALCULATION == op_on)
		{
			LCD_DisplayStringLine(214,210,"打开");
		}else{
			LCD_DisplayStringLine(214,210,"关闭");
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
		DrawInstruction("修改延时");
	}else{
		DrawInstruction("Input Range Delay");
	}
	page_flag = setdis;
	op_flag = set_delay;
}
