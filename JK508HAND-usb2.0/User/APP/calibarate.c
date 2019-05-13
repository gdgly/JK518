/**
  ******************************************************************************
  * @file    sysytem.c
  * @author  fire
  * @version V1.0
  * @date    2018-04-29
  * @brief   �ֳֶ�·�¶Ȳ����ǹ������ý���
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

float Correction[60];

extern union 
{
   unsigned char FLAG_VAL6;
	
   struct 
   {
       unsigned char  TC:4;    //�ȵ�ż����
       unsigned char  FT:1;    //��ѡ/�Ƚ�
       unsigned char  BP:1;    //Ѷ��
       unsigned char  UT:2;	   //��λ
   }BIT_FLAG;
}FLAG6;

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


void page_cal(void)
{
//	char buf[10];
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
    LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BACK);
	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/
	
		
//	DrawButton(200,420);
	DrawMenu();
	Drawcalmenu();
	DrawUnit();DrawUnit();DrawUnit();
	

	LCD_SetTextColor(LCD_COLOR_HLT);  		
	LCD_SetBackColor(LCD_COLOR_BACK);
	if(LANG == chs)
	{
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "�û�У��");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(40,10,"ͨ��");
		LCD_DisplayStringLine(40,180,"��ǰֵ");
		LCD_DisplayStringLine(40,374,"��");
		LCD_DisplayStringLine(40,510,"��λ");
	}else{
		LCD_DisplayStringLine(3,10, "<CORRECTION>");
		
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(40,10,"CHAN");
		LCD_DisplayStringLine(40,180,"VALUE");
		LCD_DisplayStringLine(40,360,"��");
		LCD_DisplayStringLine(40,510,"UNIT");
	}
	
	
	LCD_DisplayStringLine(80,10,"001");
	LCD_DisplayStringLine(120,10,"002");
	LCD_DisplayStringLine(160,10,"003");
	LCD_DisplayStringLine(200,10,"004");
	LCD_DisplayStringLine(240,10,"005");
	LCD_DisplayStringLine(280,10,"006");
	LCD_DisplayStringLine(320,10,"007");
	LCD_DisplayStringLine(360,10,"008");
	
	if(UNIT == C){
		DISP_CNL_S(80,520,"o");
		LCD_DisplayStringLine(80,525,"C");
		DISP_CNL_S(120,520,"o");
		LCD_DisplayStringLine(120,525,"C");
		DISP_CNL_S(160,520,"o");
		LCD_DisplayStringLine(160,525,"C");
		DISP_CNL_S(200,520,"o");
		LCD_DisplayStringLine(200,525,"C");
		DISP_CNL_S(240,520,"o");
		LCD_DisplayStringLine(240,525,"C");
		DISP_CNL_S(280,520,"o");
		LCD_DisplayStringLine(280,525,"C");
		DISP_CNL_S(320,520,"o");
		LCD_DisplayStringLine(320,525,"C");
		DISP_CNL_S(360,520,"o");
		LCD_DisplayStringLine(360,525,"C");
	}else if(UNIT == F){
		DISP_CNL_S(80,520,"o");
		LCD_DisplayStringLine(80,525,"F");
		DISP_CNL_S(120,520,"o");
		LCD_DisplayStringLine(120,525,"F");
		DISP_CNL_S(160,520,"o");
		LCD_DisplayStringLine(160,525,"F");
		DISP_CNL_S(200,520,"o");
		LCD_DisplayStringLine(200,525,"F");
		DISP_CNL_S(240,520,"o");
		LCD_DisplayStringLine(240,525,"F");
		DISP_CNL_S(280,520,"o");
		LCD_DisplayStringLine(280,525,"F");
		DISP_CNL_S(320,520,"o");
		LCD_DisplayStringLine(320,525,"F");
		DISP_CNL_S(360,520,"o");
		LCD_DisplayStringLine(360,525,"F");
	}else if(UNIT == K){
		LCD_DisplayStringLine(80,525,"K");
		LCD_DisplayStringLine(120,525,"K");
		LCD_DisplayStringLine(160,525,"K");
		LCD_DisplayStringLine(200,525,"K");
		LCD_DisplayStringLine(240,525,"K");
		LCD_DisplayStringLine(280,525,"K");
		LCD_DisplayStringLine(320,525,"K");
		LCD_DisplayStringLine(360,525,"K");
	}
	

	cal_disp(1,CH1TEMP - COR1);
	cal_disp(2,CH2TEMP - COR2);
	cal_disp(3,CH3TEMP - COR3);
	cal_disp(4,CH4TEMP - COR4);
	cal_disp(5,CH5TEMP - COR5);
	cal_disp(6,CH6TEMP - COR6);
	cal_disp(7,CH7TEMP - COR7);
	cal_disp(8,CH8TEMP - COR8);
	
	maxfocus_on(1,COR1);
	maxfocus_off(2,COR2);
	maxfocus_off(3,COR3);
	maxfocus_off(4,COR4);
	maxfocus_off(5,COR5);
	maxfocus_off(6,COR6);
	maxfocus_off(7,COR7);
	maxfocus_off(8,COR8);	
	
//	maxfocus_on(1,COR1);
//	maxfocus_off(2,COR2);
//	maxfocus_off(3,COR3);
//	maxfocus_off(4,COR4);
//	maxfocus_off(5,COR5);
//	maxfocus_off(6,COR6);
//	maxfocus_off(7,COR7);
//	maxfocus_off(8,COR8);
	
	if(LANG == chs)
	{
		DrawInstruction("����ֵ");
	}else{
		DrawInstruction("Input Temperature");
	}
	
//	LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
//	LCD_DrawRect(224,405,250,30);
//	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
//	LCD_DrawFullRect(225,406,249,29);
//	
//	LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
//	LCD_DrawRect(312,408,150,24);
//	LCD_SetBackColor(LCD_COLOR_WHITE);
//	LCD_SetTextColor(LCD_COLOR_BLACK);
//	DISP_CNL_S(409,240,"��������");
//	DISP_CNL_S(410,314,"325.6");
	
	page_flag = calibrate;
	op_flag = set_c1;
	bit_flag = 1;
}

