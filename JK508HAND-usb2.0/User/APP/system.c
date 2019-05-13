/**
  ******************************************************************************
  * @file    sysytem.c
  * @author  fire
  * @version V1.0
  * @date    2018-04-29
  * @brief   �ֳֶ�·�¶Ȳ�����ϵͳ���ý���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
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

void page_sys(void)
{
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/
	
	char buf[10];
	
	DrawMenu();
	Drawsysmenu();
	if(LANG == chs)
	{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "ϵͳ����");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(54,10,"����");
		LCD_DisplayStringLine(94,10,"����/ʱ��");
//		LCD_DisplayStringLine(134,10,"�˺�");
		LCD_DisplayStringLine(134,10,"����");
		LCD_DisplayStringLine(174,10,"��������");
		LCD_DisplayStringLine(214,10,"������");
		LCD_DisplayStringLine(254,10,"������");
		LCD_DisplayStringLine(294,10,"U�̶�ʱ");

	}else{
		LCD_SetTextColor(LCD_COLOR_HLT);  		
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(3,10, "<SYSTEM CONFIG>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(50,13,"LANGUAGE");
		LCD_DisplayStringLine(90,10,"DATE/TIME");
//		LCD_DisplayStringLine(130,10,"ACCOUNT");
		LCD_DisplayStringLine(130,10,"BRIGHTNESS");
		LCD_DisplayStringLine(170,10,"DIM BRIGHT");
		LCD_DisplayStringLine(210,10,"TOUCH PANEL");
		LCD_DisplayStringLine(250,10,"BAUND");
		LCD_DisplayStringLine(290,10,"USB TIME");

	}
	
	page_flag = sysset;
	op_flag = set_lang;
	Disp_sys();
}

void Disp_sys(void)
{
	char buf[10];
	
	if(op_flag == set_lang)
	{
		if(LANG == chs)
		{
			DrawInstruction("ϵͳ����ѡ��");
		}else{
			DrawInstruction("Select system language");
		}
		focus_on1();
		
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		LCD_DisplayStringLine(50,170,"CHINESE");
	}else{
		LCD_DisplayStringLine(50,170,"ENGLISH");
	}
	
	if(op_flag == set_brt)
	{
		if(LANG == chs)
		{
			DrawInstruction("������������");
		}else{
			DrawInstruction("Set brightness");
		}
		focus_on1();	
	}else{
		focus_off1();
	}
	if(BRTS == L0)
	{
		brightness = 10;
		sprintf(buf,"%d",10);
	}else if(BRTS == L1){
		brightness = 20;
		sprintf(buf,"%d",25);
	}else if(BRTS == L2){
		brightness = 40;
		sprintf(buf,"%d",50);
	}else if(BRTS == L3){
		brightness = 60;
		sprintf(buf,"%d",75);
	}else if(BRTS == L4){
		brightness = 80;
		sprintf(buf,"%d",100);
	}		
	LCD_DisplayStringLine(130,170,(uint8_t*)buf);
	if(op_flag == set_date)
	{
		if(LANG == chs)
		{
			DrawInstruction("��-��-��");
		}else{
			DrawInstruction("YEAR-MONTH-DAY");
		}
	}
	if(op_flag == set_dim)
	{
		if(LANG == chs)
		{
			DrawInstruction("��������ʱ��");
		}else{
			DrawInstruction("Reduce brightness");
		}
		focus_on1();
		
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		if(DIM == DOFF)
		{
			LCD_DisplayStringLine(174,170,"�ر�");
		}else if(DIM == D5){
			sprintf(buf,"%d",5);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D10){
			sprintf(buf,"%d",10);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D15){
			sprintf(buf,"%d",15);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D30){
			sprintf(buf,"%d",30);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}
	}else if(LANG  == eng){
		if(DIM == DOFF)
		{
			LCD_DisplayStringLine(172,170,"OFF");
		}else if(DIM == D5){
			sprintf(buf,"%d",5);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D10){
			sprintf(buf,"%d",10);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D15){
			sprintf(buf,"%d",15);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}else if(DIM == D30){
			sprintf(buf,"%d",30);
			LCD_DisplayStringLine(172,170,(uint8_t*)buf);
		}
	}
	
	if(op_flag == set_touch)
	{
		if(LANG == chs)
		{
			DrawInstruction("��������");
		}else{
			DrawInstruction("Touch panel");
		}
		focus_on1();
		
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		if(TOUCH == op_on)
		{
			LCD_DisplayStringLine(214,170,"��");
		}else{
			LCD_DisplayStringLine(214,170,"�ر�");
		}
	}else if(LANG  == eng){
		if(TOUCH == op_on)
		{
			LCD_DisplayStringLine(212,170,"ON");
		}else{
			LCD_DisplayStringLine(212,170,"OFF");
		}
	}
	
	if(op_flag == set_baud)
	{
		if(LANG == chs)
		{
			DrawInstruction("����������");
		}else{
			DrawInstruction("Select Baud rate");
		}
		focus_on1();
		
	}else{
		focus_off1();
	}
	if(BAUD == b9600)
	{
		LCD_DisplayStringLine(252,170,"9600  ");
	}else if(BAUD == b19200){
		LCD_DisplayStringLine(252,170,"19200 ");
	}else if(BAUD == b38400){
		LCD_DisplayStringLine(252,170,"38400 ");
	}else if(BAUD == b57600){
		LCD_DisplayStringLine(252,170,"57600 ");
	}else if(BAUD == b115200){
		LCD_DisplayStringLine(252,170,"115200");
	}
	
	if(op_flag == set_timer)
	{
		stimefocus_on(SAVETIME);
	}else{
		stimefocus_off(SAVETIME);
	}
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	LCD_DisplayStringLine(130,230,"%");
	LCD_DisplayStringLine(291,218,"S");
}

void Disp_sysset_button(void)
{
	switch(op_flag)
	{
		case set_lang:
		{
			DrawMenu();
			Drawlang();
			op_sw = op_on;
		}break;
		case set_date:
		{
			DrawMenu();
			Drawdate();
			op_sw = op_on;
		}break;
		case set_time:
		{
			DrawMenu();
			Drawtime();
			op_sw = op_on;
		}break;
		case set_brt:
		{
			DrawMenu();
			DrawBrt();
			op_sw = op_on;
		}break;
		case set_dim:
		{
			DrawMenu();
			DrawDim();
			op_sw = op_on;
		}break;
		case set_touch:
		{
			DrawMenu();
			DrawTouch();
			op_sw = op_on;
		}break;
		case set_baud:
		{
			DrawMenu();//�ػ�˵���
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_DisplayStringLine(445,30,"9600");
			LCD_DisplayStringLine(445,150,"19200");
			LCD_DisplayStringLine(445,280,"38400");
			LCD_DisplayStringLine(445,405,"57600");
			LCD_DisplayStringLine(445,525,"115200");
			op_sw = op_on;
		}break;
		case set_timer:
		{
			SAVETIME = atoi(data);
			if(SAVETIME > 255)
			{
				SAVETIME = 255;
			}
			stimefocus_on(SAVETIME);
			Save_flag();
		}break;
	}
}