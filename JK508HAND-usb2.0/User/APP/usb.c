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


char filename[16][10]={0};
u8 filecur = 0; //��ռ�õ��ļ���
char newname[10]={0};
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
       unsigned char  LG:1;	   	//����
   }BIT_FLAG;
}FLAG7;


void page_usb(void)
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
//	Drawusbmenu();

	

	LCD_SetTextColor(LCD_COLOR_HLT);  		
	LCD_SetBackColor(LCD_COLOR_BACK);
	if(LANG == chs)
	{
		LCD_DisplayStringLine(0,10, "<      >");
		LCD_DisplayStringLine(5,26, "�ļ�����");
		LCD_DisplayStringLine(5,170, "���ļ�");
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(40,10,"���");
		LCD_DisplayStringLine(40,180,"�ļ���");
		LCD_DisplayStringLine(40,350,"���");
		LCD_DisplayStringLine(40,510,"�ļ���");
	}else{
		LCD_DisplayStringLine(3,130, "NewFile");
		LCD_DisplayStringLine(3,10, "<FILE>");
		LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);	
		LCD_SetBackColor(LCD_COLOR_BACK);
		LCD_DisplayStringLine(40,10,"No.");
		LCD_DisplayStringLine(40,180,"NAME");
		LCD_DisplayStringLine(40,360,"No.");
		LCD_DisplayStringLine(40,510,"NAME");
	}
	
	
	LCD_DisplayStringLine(80,10,"01");
	LCD_DisplayStringLine(120,10,"02");
	LCD_DisplayStringLine(160,10,"03");
	LCD_DisplayStringLine(200,10,"04");
	LCD_DisplayStringLine(240,10,"05");
	LCD_DisplayStringLine(280,10,"06");
	LCD_DisplayStringLine(320,10,"07");
	LCD_DisplayStringLine(360,10,"08");
	
	
	LCD_DisplayStringLine(80,360,"09");
	LCD_DisplayStringLine(120,360,"10");
	LCD_DisplayStringLine(160,360,"11");
	LCD_DisplayStringLine(200,360,"12");
	LCD_DisplayStringLine(240,360,"13");
	LCD_DisplayStringLine(280,360,"14");
	LCD_DisplayStringLine(320,360,"15");
	LCD_DisplayStringLine(360,360,"16");
  Newfocus_on(newname);
	

	leftfocus_off(1,filename[0]);
	leftfocus_off(2,filename[1]);
	leftfocus_off(3,filename[2]);
	leftfocus_off(4,filename[3]);
	leftfocus_off(5,filename[4]);
	leftfocus_off(6,filename[5]);
	leftfocus_off(7,filename[6]);
	leftfocus_off(8,filename[7]);
	
	rightfocus_off(1,filename[8]);
	rightfocus_off(2,filename[9]);
	rightfocus_off(3,filename[10]);
	rightfocus_off(4,filename[11]);
	rightfocus_off(5,filename[12]);
	rightfocus_off(6,filename[13]);
	rightfocus_off(7,filename[14]);
	rightfocus_off(8,filename[15]);	
	
//	if(LANG == chs)
//	{
//		DrawInstruction("��������");
//	}else{
//		DrawInstruction("Low limit");
//	}		
	
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
	
	page_flag = usbmanu;
	op_flag = set_min0;
	bit_flag = 1;
}
