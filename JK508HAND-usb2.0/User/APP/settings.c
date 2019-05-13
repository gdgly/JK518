/**
  ******************************************************************************
  * @file    sysytem.c
  * @author  fire
  * @version V1.0
  * @date    2018-04-29
  * @brief   手持多路温度测试仪功能设置界面
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
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
   unsigned char FLAG_VAL6;
	
   struct 
   {
       unsigned char  TC:4;    //热电偶类型
       unsigned char  FT:1;    //分选/比较
       unsigned char  BP:1;    //讯响
       unsigned char  UT:2;	   //单位
   }BIT_FLAG;
}FLAG6;

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


void page_set(void)
{
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
	

	DrawMenu();
	Drawsetmenu();
	DrawUnit();
	
	LCD_SetTextColor(LCD_COLOR_HLT);  		
	LCD_SetBackColor(LCD_COLOR_BACK);
	if(LANG == chs)
	{
		LCD_DisplayStringLine(0,10, "<        >");
		LCD_DisplayStringLine(5,30, "比较器设置");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,10,"比较");
		LCD_DisplayStringLine(90,10,"讯响");
		LCD_DisplayStringLine(130,10,"方式");
		LCD_DisplayStringLine(170,10,"下限");
//		LCD_DisplayStringLine(170,10,"字体");
//		LCD_DisplayStringLine(280,10,"U盘设置");
//		LCD_DisplayStringLine(320,10,"定时");
		LCD_DisplayStringLine(50,360,"速度");
		LCD_DisplayStringLine(90,360,"标称");
		LCD_DisplayStringLine(174,360,"上");
		LCD_DisplayStringLine(174,384,"限");
//		LCD_DisplayStringLine(320,360,"定时");
	}else{
		LCD_DisplayStringLine(3,10, "<COMP SETUP>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,10,"COMP");
		LCD_DisplayStringLine(90,10,"BEEP");
		LCD_DisplayStringLine(127,10,"MODE");
		LCD_DisplayStringLine(170,10,"LOWER");
//		LCD_DisplayStringLine(170,10,"FONT");
//		LCD_DisplayStringLine(280,10,"USBHDD SET");
//		LCD_DisplayStringLine(317,10,"TIME");
		LCD_DisplayStringLine(50,360,"SPEED");
		LCD_DisplayStringLine(90,360,"NOM");
		LCD_DisplayStringLine(170,360,"UPPER");
//		LCD_DisplayStringLine(320,360,"TIME");
	}
	
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(FILTER == ft_off){
		if(LANG == chs)
		{
			LCD_DisplayStringLine(50,150,"关闭");
		}else{
			LCD_DisplayStringLine(50,150,"OFF");
		}
	}else{
		if(LANG == chs)
		{
			LCD_DisplayStringLine(50,150,"打开");
		}else{
			LCD_DisplayStringLine(50,150,"ON");
		}
	}
	
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	if(BEEP == beep_off){
		if(LANG == chs)
		{
			LCD_DisplayStringLine(90,150,"关闭");
		}else{
			LCD_DisplayStringLine(90,150,"OFF");
		}
	}else{
		if(LANG == chs)
		{
			LCD_DisplayStringLine(90,150,"打开");
		}else{
			LCD_DisplayStringLine(90,150,"ON");
		}
	}
	if(LANG == chs)
	{
		if(UNIT == C){
			LCD_SetTextColor(LCD_COLOR_YELLOW);
	//		DISP_CNL_S(127,150,"o");
			LCD_DisplayStringLine(130,150,"绝对值爵");
		}else if(UNIT == K){
			LCD_SetTextColor(LCD_COLOR_YELLOW);
	//		DISP_CNL_S(127,150,"o");
			LCD_DisplayStringLine(130,150,"百分比粉");
		}else{
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_DisplayStringLine(130,150,"直读值");
			LCD_DisplayStringLine(127,225,"SEQ");
		}
	}else{
		if(UNIT == C){
			LCD_SetTextColor(LCD_COLOR_YELLOW);
	//		DISP_CNL_S(127,150,"o");
			LCD_DisplayStringLine(127,150,"ABS~");
		//	LCD_DisplayStringLine(130,195,"爵");
		}else if(UNIT == K){
			LCD_SetTextColor(LCD_COLOR_YELLOW);
	//		DISP_CNL_S(127,150,"o");
			LCD_DisplayStringLine(127,150,"PRE%");
		//	LCD_DisplayStringLine(130,195,"粉");
		}else{
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_DisplayStringLine(127,150,"D-SEQ");
		}
	}
//	if(FONT == big){
//		if(LANG == chs)
//		{
//			LCD_DisplayStringLine(170,150,"大");
//		}else{
//			LCD_DisplayStringLine(170,150,"L");
//		}
//	}else if(FONT == middle){
//		if(LANG == chs)
//		{
//			LCD_DisplayStringLine(170,150,"中");
//		}else{
//			LCD_DisplayStringLine(170,150,"M");
//		}
//	}else if(FONT == small){
//		if(LANG == chs)
//		{
//			LCD_DisplayStringLine(170,150,"小");
//		}else{
//			LCD_DisplayStringLine(170,150,"S");
//		}
//	}
	
//	LCD_DisplayStringLine(317,150,"AUTO");
	
	if(SPEED == fast)
	{
		if(LANG == chs)
		{
			LCD_DisplayStringLine(50,500,"快速");
		}else{
			LCD_DisplayStringLine(50,500,"FAST");
		}
	}else if(SPEED == moderate){
		if(LANG == chs)
		{
			LCD_DisplayStringLine(50,500,"中速");
		}else{
			LCD_DisplayStringLine(50,500,"MED");
		}
	}else if(SPEED == slow){
		if(LANG == chs)
		{
			LCD_DisplayStringLine(50,500,"慢速");
		}else{
			LCD_DisplayStringLine(50,500,"SLOW");
		}
	}
/*	
	if(BAUD == b9600)
	{
		LCD_DisplayStringLine(87,500,"9600");
	}else if(BAUD == b19200){
		LCD_DisplayStringLine(87,500,"19200");
	}else if(BAUD == b38400){
		LCD_DisplayStringLine(87,500,"38400");
	}else if(BAUD == b57600){
		LCD_DisplayStringLine(87,500,"57600");
	}else if(BAUD == b115200){
		LCD_DisplayStringLine(87,500,"115200");
	}
*/
	if(NOM== 0)
	{
		LCD_DisplayStringLine(90,605,"m#");
	}else if(NOM == 1){
		LCD_DisplayStringLine(90,605,"#");
	}else if(NOM == 2){
		LCD_DisplayStringLine(90,605,"k#");
	}else{
		LCD_DisplayStringLine(90,605,"M#");
	}
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(170,255,50,31);	
						LCD_DrawFullRect(170,605,50,31);
	/*
	if(UNIT == C)
	{
		if(LOWERNOM== 0)
		{
			LCD_DisplayStringLine(170,355,"m#");
		}else if(LOWERNOM == 1){
			LCD_DisplayStringLine(170,355,"#");
		}else if(LOWERNOM == 2){
			LCD_DisplayStringLine(170,355,"k#");
		}else{
			LCD_DisplayStringLine(170,355,"M#");
		}
		if(UPPERNOM== 0)
		{
			LCD_DisplayStringLine(170,605,"m#");
		}else if(UPPERNOM == 1){
			LCD_DisplayStringLine(170,605,"#");
		}else if(UPPERNOM == 2){
			LCD_DisplayStringLine(170,605,"k#");
		}else{
			LCD_DisplayStringLine(170,605,"M#");
		}
	}
	else */if(UNIT == K)
	{
		LCD_DisplayStringLine(170,255,"%");
		LCD_DisplayStringLine(170,605,"%");
	}
	LOWER=0.3;
	UPPER=4569.333;
	NOMA=342.4532;
	   comminfocus_off(LOWER);
		commaxfocus_off(UPPER);
		nomafocus_off(NOMA);
	
//	LCD_DisplayStringLine(317,150,"1");
//	stimefocus_off(SAVETIME);
//	LCD_DisplayStringLine(319,166,"S");
	
	if(LANG == chs)
	{
		DrawInstruction("比较器设置");
	}else{
		DrawInstruction("Comparator ON/OFF");
	}		
	
	page_flag = settings;
	op_flag = set_filter;
	bit_flag = 1;
}
