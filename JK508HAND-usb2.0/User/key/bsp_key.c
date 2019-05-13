/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./beep/bsp_beep.h"
#include "./flash/bsp_spi_flash.h"
#include "./tim/bsp_basic_tim.h"
#include "./RTC/bsp_rtc.h"
#include "./ch376/ch376.h"
#include "./FILESYS/FILESYS.h"
#include "jk508.h"
#include "./usart/bsp_debug_usart.h"

u8 key_value;
extern u16 count;
u8 count_flag = 0;
u8 page_flag;
u8 op_flag;
u8 ch_page;
u8 tc_type;
u8 op_sw;
u8 spt_page;
u8 cor_page;
u8 date_page;
u8 time_page;
u8 bit_flag = 1;
u8 dot_flag = 0;
u8 neg_flag = 0;
u8 input_flag = 0;
u16 hpage;
char data[17];
float offset[40];
u8 keystat;
u8 date_time[6] = {0,1,1,0,0,0};

u8 fileflag;
extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW1:1;
       unsigned char  SW2:1;
       unsigned char  SW3:1;
       unsigned char  SW4:1;
       unsigned char  SW5:1;
       unsigned char  SW6:1;
       unsigned char  SW7:1;
       unsigned char  SW8:1; 
   }BIT_FLAG;
}FLAG1;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW9:1;
       unsigned char  SW10:1;
       unsigned char  SW11:1;
       unsigned char  SW12:1;
       unsigned char  SW13:1;
       unsigned char  SW14:1;
       unsigned char  SW15:1;
       unsigned char  SW16:1;  
   }BIT_FLAG;
}FLAG2;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW17:1;
       unsigned char  SW18:1;
       unsigned char  SW19:1;
       unsigned char  SW20:1;
       unsigned char  SW21:1;
       unsigned char  SW22:1;
       unsigned char  SW23:1;
       unsigned char  SW24:1;  
   }BIT_FLAG;
}FLAG3;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW25:1;
       unsigned char  SW26:1;
       unsigned char  SW27:1;
       unsigned char  SW28:1;
       unsigned char  SW29:1;
       unsigned char  SW30:1;
       unsigned char  SW31:1;
       unsigned char  SW32:1;
   }BIT_FLAG;
}FLAG4;

extern union 
{
   unsigned char FLAG_VAL;
		
   struct 
   {
       unsigned char  SW33:1;
       unsigned char  SW34:1;
       unsigned char  SW35:1;
       unsigned char  SW36:1;
       unsigned char  SW37:1;
       unsigned char  SW38:1;
       unsigned char  SW39:1;	
       unsigned char  SW40:1;
   }BIT_FLAG;
}FLAG5;

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

extern union 
{
   unsigned char FLAG_VAL8;
	
   struct 
   {
       unsigned char  BR:3;    //亮度
       unsigned char  DM:3;    //降低亮度
       
   }BIT_FLAG;
}FLAG8;

/// 不精确的延时
void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
}


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
//static void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  
//  /* 配置NVIC为优先级组1 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//  
//  /* 配置中断源：按键1 */
//  NVIC_InitStructure.NVIC_IRQChannel = P6_EXTI_IRQ;
//  /* 配置抢占优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  /* 配置子优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  /* 使能中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//}


void Esc_menu(void)
{
	DrawMenu();
	if(page_flag == manus)
	{
		Drawhomemenu();
	}else if(page_flag == sysset){
		Drawsysmenu();
	}
	op_sw = op_off;
}

/*得到焦点*/
void focus_on(u8 flag)
{
	if(flag == ch_on)
	{
		LCD_SetBackColor(LCD_COLOR_HLT);
		LCD_SetTextColor(LCD_COLOR_BLACK);
	}else{
		LCD_SetBackColor(LCD_COLOR_LIGHTGREY);
		LCD_SetTextColor(LCD_COLOR_BLACK);
	}	
}

/*失去焦点*/
void focus_off(u8 flag)
{
	switch(page_flag)
	{
		case display:
		{
			if(flag == ch_on)
			{	
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_HLT);
			}else{
				LCD_SetBackColor(LCD_COLOR_BACK);
				LCD_SetTextColor(LCD_COLOR_LIGHTGREY);
			}
		}break;
		
		case settings:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
	}
}

void focus_on1(void)
{
	switch(page_flag)
	{		
		case settings:
		{			
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
		case sysset:
		{
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
		case display:
		{
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
		case manus:
		{
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
		case setdis:
		{
			LCD_SetBackColor(LCD_COLOR_YELLOW);
			LCD_SetTextColor(LCD_COLOR_BLACK);
		}break;
	}
}

void focus_off1(void)
{
	switch(page_flag)
	{		 
		case settings:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
		case sysset:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
		case display:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
		case manus:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
		case setdis:
		{
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
		}break;
	}
}

void minfocus_on(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(153,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("下限设置");
	}else{
		DrawInstruction("Low limit");
	}
	clear_input();
}

void minfocus_off(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(153,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
	}
}


void Newfocus_on(char* name)
{
//s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(250,3,160,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	strcpy(buf,name);
	LCD_DisplayStringLine(3,250,(uint8_t* )buf);
	Drawfileusbmenu();
	
	if(LANG == chs)
	{
		DrawInstruction("新建文件");
	}else{
		DrawInstruction("New file");
	}
	DrawMenu();
	LCD_SetTextColor(LCD_COLOR_BT);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	if(LANG == chs)
	{
		LCD_DisplayStringLine(450,35,"新建");
	}else{
		LCD_DisplayStringLine(450,30,"NEW");
	}
	if(LANG == chs)
	{

		LCD_DisplayStringLine(450,420,"显示");
		LCD_DisplayStringLine(450,548,"系统");

	}else{

		LCD_DisplayStringLine(450,394,"DISPLAY");
		LCD_DisplayStringLine(450,535,"SYSTEM");

	}
	clear_input();
}


void Newfocus_off(char* name)
{
//	s16 i = abs(lmt)/10;
	char buf[10]={0};
//	DrawMenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(250,3,160,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	strcpy(buf,name);
	LCD_DisplayStringLine(3,250,(uint8_t* )buf);
}

void leftfocus_on(u8 c,char* name)
{
//s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(143,40+c*40,160,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	strcpy(buf,name);
	LCD_DisplayStringLine(40+c*40,143,(uint8_t* )buf);
	Drawfileusbmenu();

//	if(i == 0)
//	{
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}
//	}else if(i >=1 && i < 10){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}
//	}else if(i >=10 && i < 100){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//		}
//	}else if(i >=100 && i < 1000){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//	}
	if(LANG == chs)
	{
		DrawInstruction("文件设置");
	}else{
		DrawInstruction("Set file");
	}
	clear_input();
}

void leftfocus_off(u8 c,char* name)
{
//	s16 i = abs(lmt)/10;
	char buf[10]={0};
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(143,40+c*40,160,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	strcpy(buf,name);
	LCD_DisplayStringLine(40+c*40,143,(uint8_t* )buf);
//	if(i == 0)
//	{
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}
//	}else if(i >=1 && i < 10){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}
//	}else if(i >=10 && i < 100){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//		}
//	}else if(i >=100 && i < 1000){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//	}
}


void tc_parfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(210,90,112,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
		if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,226,(uint8_t* )buf);
		}
	}else if(i >=10 && i <= 100){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,226,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,210,(uint8_t* )buf);
		}
	}
	if(LANG == chs)
	{
		DrawInstruction("温度系数设置");
	}else{
		DrawInstruction("Set TC-PAR");
	}
	clear_input();
}

void tc_parfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(210,90,112,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
		if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,226,(uint8_t* )buf);
		}
	}else if(i >=10 && i <= 100){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,226,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(90,210,(uint8_t* )buf);
		}
	}
}

void tc_tepfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(210,130,112,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,264,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,248,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,232,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,226,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,226,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,210,(uint8_t* )buf);
		}
	}
}

void tc_tepfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(210,130,112,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,264,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,248,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,232,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,226,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
		if(lmt >= 0){
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,226,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.3f",lmt);
			LCD_DisplayStringLine(130,210,(uint8_t* )buf);
		}
	}
	if(LANG == chs)
	{
		DrawInstruction("参考温度设置");
	}else{
		DrawInstruction("Set TC-TEP");
	}
	clear_input();
}






void consfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(210,250,112,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,201+57,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,185+57,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,185+57,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,169+57,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,169+57,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,153+57,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,153+57,(uint8_t* )buf);
	}
}

void consfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(210,250,112,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,201+57,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,185+57,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,185+57,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,169+57,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,169+57,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,153+57,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(250,153+57,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("电极常数设置");
	}else{
		DrawInstruction("Set Constant");
	}
	clear_input();
}

void Rfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(250,170,138,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,301-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,285-50,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,285-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,269-50,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,269-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,253-50,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,253-50,(uint8_t* )buf);
	}

	LCD_DisplayStringLine(170,420-60,"#*cm");
	clear_input();
}

void Rfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(250-50,170,138,31);
	//LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,301-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,285-50,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,285-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,269-50,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,269-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,253-50,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,253-50,(uint8_t* )buf);
	}
	LCD_DisplayStringLine(170,420-60,"#*cm");
}


void Cfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(250-50,210,138,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,301-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,285-50,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,285-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,269-50,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,269-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,253-50,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,253-50,(uint8_t* )buf);
	}
  LCD_DisplayStringLine(210,420-60,"mS/cm");
	clear_input();
}

void Cfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,210,138,31);
//	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,301-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,285-50,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,285-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,269-50,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,269-50,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,253-50,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(210,253-50,(uint8_t* )buf);
	}
	LCD_DisplayStringLine(210,420-60,"mS/cm");
}




void comminfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(150,170,106,31);
	//LCD_DrawFullRect(335,170,48,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,201,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,185,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,185,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,169,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,153,(uint8_t* )buf);
	}
	if(UNIT == C)
	{
			if(LOWERNOM== 0)
			{
					LCD_DisplayStringLine(170,255,"m#");
			}else if(LOWERNOM == 1){
			LCD_DisplayStringLine(170,255,"#");
			}else if(LOWERNOM == 2){
			LCD_DisplayStringLine(170,255,"k#");
			}else{
					LCD_DisplayStringLine(170,255,"M#");
			}					
	}
	if(LANG == chs)
	{
		DrawInstruction("下限设置");
	}else{
		DrawInstruction("Lower limit");
	}
	clear_input();
}

void comminfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(150,170,106,31);
	//LCD_DrawFullRect(335,170,48,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,201,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,185,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,185,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,169,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,153,(uint8_t* )buf);
	}
	if(UNIT == C)
	{
			if(LOWERNOM== 0)
			{
					LCD_DisplayStringLine(170,255,"m#");
			}else if(LOWERNOM == 1){
			LCD_DisplayStringLine(170,255,"#");
			}else if(LOWERNOM == 2){
			LCD_DisplayStringLine(170,255,"k#");
			}else{
					LCD_DisplayStringLine(170,255,"M#");
			}					
	}
}


void commaxfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(500,170,106,31);
	//LCD_DrawFullRect(605,170,48,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,551,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,535,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,535,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,519,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,519,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,503,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,503,(uint8_t* )buf);
	}
	if(UNIT == C)
	{
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
	if(LANG == chs)
	{
		DrawInstruction("上限设置");
	}else{
		DrawInstruction("Upper limit");
	}
	clear_input();
}

void commaxfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(500,170,106,31);
	//LCD_DrawFullRect(605,170,48,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,551,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,535,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,535,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,519,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,519,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,503,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(170,503,(uint8_t* )buf);
	}
	if(UNIT == C)
	{
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
}




void nomafocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(500,90,106,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,551,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,535,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,535,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,519,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,519,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,503,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,503,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("标称设置");
	}else{
		DrawInstruction("Input nominal value");
	}
	clear_input();
}

void nomafocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(500,90,106,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,551,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,535,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,535,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,519,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,519,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,503,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,503,(uint8_t* )buf);
	}
}

void cal_disp(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
//	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
//	LCD_DrawFullRect(153,40+c*40,96,31);
//	LCD_SetTextColor(LCD_COLOR_YELLOW);
//	LCD_SetBackColor(LCD_COLOR_BACK);
	
//	if(i == 0)
//	{
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,201,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}
//	}else if(i >=1 && i < 10){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,185,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}
//	}else if(i >=10 && i < 100){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//		}
//	}else if(i >=100 && i < 1000){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
//	}
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			strcat(buf,"   ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			strcat(buf,"  ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			strcat(buf,"  ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			strcat(buf," ");
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			strcat(buf," ");
			LCD_DisplayStringLine(40+c*40,169,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,153,(uint8_t* )buf);
	}
}

void maxfocus_on(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(323,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,371,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("上限设置");
	}else{
		DrawInstruction("High limit");
	}		
	clear_input();
}

void maxfocus_off(u8 c,float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(323,40+c*40,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,371,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
	}
}

void rightfocus_on(u8 c,char* name)
{
//	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(500-27,40+c*40,160,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	strcpy(buf,name);
	LCD_DisplayStringLine(40+c*40,500-27,(uint8_t* )buf);
	Drawfileusbmenu();
//	if(i == 0)
//	{
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,371,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
//		}
//	}else if(i >=1 && i < 10){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
//		}
//	}else if(i >=10 && i < 100){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
//		}
//	}else if(i >=100 && i < 1000){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
//	}
	if(LANG == chs)
	{
		DrawInstruction("文件设置");
	}else{
		DrawInstruction("Set file");
	}
	clear_input();
}

void rightfocus_off(u8 c,char* name)
{
//	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(500-27,40+c*40,160,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	strcpy(buf,name);
	LCD_DisplayStringLine(40+c*40,500-27,(uint8_t* )buf);
//	if(i == 0)
//	{
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,371,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
//		}
//	}else if(i >=1 && i < 10){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,355,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
//		}
//	}else if(i >=10 && i < 100){
//		if(lmt >= 0){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,339,(uint8_t* )buf);
//		}else{
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
//		}
//	}else if(i >=100 && i < 1000){
//			sprintf(buf,"%.1f",lmt);
//			LCD_DisplayStringLine(40+c*40,323,(uint8_t* )buf);
//	}
}

void yhfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(200,50,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("Y轴上限");
	}else{
		DrawInstruction("Y-High Value");
	}		
	clear_input();
}

void yhfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,50,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(50,200,(uint8_t* )buf);
	}
}

void ylfocus_on(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(200,90,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("Y轴下限");
	}else{
		DrawInstruction("Y-Low Value");
	}		
	clear_input();
}

void ylfocus_off(float lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,90,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,248,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}
	}else if(i >=1 && i < 10){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,232,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}
	}else if(i >=10 && i < 100){
		if(lmt >= 0){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,216,(uint8_t* )buf);
		}else{
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
		}
	}else if(i >=100 && i < 1000){
			sprintf(buf,"%.1f",lmt);
			LCD_DisplayStringLine(90,200,(uint8_t* )buf);
	}
}

void multifocus_on(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(200,130,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,280,(uint8_t* )buf);
	
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,264,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,248,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("采样数");
	}else{
		DrawInstruction("Multi Value");
	}		
	clear_input();
}

void multifocus_off(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(200,130,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,280,(uint8_t* )buf);
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,264,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(130,248,(uint8_t* )buf);
	}
}

void stimefocus_on(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(170,290,48,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(290,202,(uint8_t* )buf);
	
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(290,186,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(290,170,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("记录间隔时间");
	}else{
		DrawInstruction("Record interval");
	}		
	clear_input();
}

void stimefocus_off(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,290,48,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(290,202,(uint8_t* )buf);
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(290,186,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(290,170,(uint8_t* )buf);
	}
}


void delaytimefocus_on(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
	LCD_DrawFullRect(210,170,96,31);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_YELLOW);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,258,(uint8_t* )buf);	
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,242,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,226,(uint8_t* )buf);
	}else if(i >=100 && i < 1000){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,210,(uint8_t* )buf);
	}
	if(LANG == chs)
	{
		DrawInstruction("修改延时");
	}else{
		DrawInstruction("Input Range Delay");
	}		
	clear_input();
}

void delaytimefocus_off(int lmt)
{
	s16 i = abs(lmt)/10;
	char buf[10];
	
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(210,170,96,31);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BACK);
	
	if(i == 0)
	{		
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,258,(uint8_t* )buf);	
	}else if(i >=1 && i < 10){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,242,(uint8_t* )buf);
	}else if(i >=10 && i < 100){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,226,(uint8_t* )buf);
	}else if(i >=100 && i < 1000){
		sprintf(buf,"%d",lmt);
		LCD_DisplayStringLine(170,210,(uint8_t* )buf);
	}
}


//输入数字
void input_num(char* num)
{
	if(bit_flag < 18){
		if(bit_flag == 1)
		{
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(224,405,250,30);
			LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
			LCD_DrawFullRect(225,406,249,29);
			
			LCD_SetColors(LCD_COLOR_LIGHTGREY,LCD_COLOR_LIGHTGREY);
			LCD_DrawRect(312,408,150,24); 
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(409,240,"输入数据");
		}
		
		strcat(data,num);
		input_flag = 1;
		if(dot_flag != 0 && strcmp(num,".") == 0)
		{
			
		}else{			
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_WHITE);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			//LCD_DrawFullRect(306 + bit_flag*9,410,9,20);
			LCD_SetBackColor(LCD_COLOR_WHITE);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,306 + bit_flag*8,(uint8_t*)num);
			LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_SetTextColor(LCD_COLOR_BLACK);
			DISP_CNL_S(410,314 + bit_flag*8," ");
			
			if(dot_flag == 0 && strcmp(num,".") == 0)
			{
				dot_flag = bit_flag;
			}
			bit_flag ++;
		}
	}
}

//清除输入框
void clear_input(void)
{
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(224,405,251,31);
	bit_flag = 1;
	dot_flag = 0;
	input_flag = 0;
	memset(data, '\0', sizeof(data));
}

//删除数字
void del_num(void)
{
	if(bit_flag > 1)
	{
		LCD_SetBackColor(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		DISP_CNL_S(410,306 + bit_flag*8," ");
		LCD_SetBackColor(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		DISP_CNL_S(410,298 + bit_flag*8," ");
		bit_flag --;
		data[bit_flag-1] = '\0';
		if(bit_flag == dot_flag)
		{
			dot_flag = 0;
		}
		if(bit_flag == 2)
		{
			neg_flag = 0;
		}
	}else if(bit_flag == 1){
		input_num("-");
		neg_flag = 1;
	}
}

//设置日期

void plus_year(void)
{
	RTC_DateTypeDef RTC_DateStructure;
		
	
	YEAR = YEAR + 1;
	if(YEAR > 99)
	{
		YEAR = 0;
	}
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_year(void)
{
	RTC_DateTypeDef RTC_DateStructure;
		
	
	YEAR = YEAR - 1;
	if(YEAR < 1)
	{
		YEAR = 1;
	}   
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_mon(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	MONTH++;
	if(MONTH > 12)
	{
		MONTH = 1;
	}    
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_mon(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	MONTH--;
	if(MONTH <1)
	{
		MONTH = 12;
	}    
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_day(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	DATE++;
	if(DATE > 31)
	{
		DATE = 1;
	}    
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_day(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	DATE--;
	if(DATE < 1)
	{
		DATE = 31;
	}
	RTC_DateStructure.RTC_Date = DATE;
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_hour(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	HOURS++;
	if(HOURS > 23)
	{
		HOURS = 0;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_hour(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	HOURS--;
	if(HOURS == 255)
	{
		HOURS = 23;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_min(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	MINUTES++;
	if(MINUTES > 59)
	{
		MINUTES = 0;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_min(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	MINUTES--;
	if(MINUTES == 255)
	{
		MINUTES = 59;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void plus_sec(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	SECONDS++;
	if(SECONDS > 59)
	{
		SECONDS = 0;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void minus_sec(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	SECONDS--;
	if(SECONDS == 255)
	{
		SECONDS = 59;
	}
	
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/*背光设置*/
void brt_set(u8 key)
{
	DrawMenu();//重绘菜单栏
	Drawsysmenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,130,55,32);
	if(press != 1)
	{
		focus_on1();
	}else{
		focus_off1();
	}
	if(key == 1)
	{
		LCD_DisplayStringLine(130,170,"10");
		BRTS = L0;
		brightness = 10;
	}else if(key == 2){
		LCD_DisplayStringLine(130,170,"25");
		BRTS = L1;
		brightness = 20;
	}else if(key == 3){
		LCD_DisplayStringLine(130,170,"50");
		BRTS = L2;
		brightness = 40;
	}else if(key == 4){
		LCD_DisplayStringLine(130,170,"75");
		BRTS = L3;
		brightness = 60;
	}else if(key == 5){
		LCD_DisplayStringLine(130,170,"100");
		BRTS = L4;
		brightness = 80;
	}
	TIM_PWMOUTPUT_Config(brightness);
	op_sw = op_off;
	Save_flag();
}

/*降低亮度设置*/
void dim_set(u8 key)
{
	DrawMenu();//重绘菜单栏
	Drawsysmenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,172,55,32);
	if(press != 1)
	{
		focus_on1();
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		if(key == 1)
		{
			LCD_DisplayStringLine(174,170,"关闭");
			LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
			LCD_DrawFullRect(230,172,55,32);
			DIM = DOFF;
		}else if(key == 2){
			LCD_DisplayStringLine(172,170,"5");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D5;
		}else if(key == 3){
			LCD_DisplayStringLine(172,170,"10");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D10;
		}else if(key == 4){
			LCD_DisplayStringLine(172,170,"15");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D15;
		}else if(key == 5){
			LCD_DisplayStringLine(172,170,"30");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D30;
		}
	}else if(LANG == eng){
		if(key == 1)
		{
			LCD_DisplayStringLine(174,170,"OFF");
			LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
			LCD_DrawFullRect(230,172,55,32);
			DIM = DOFF;
		}else if(key == 2){
			LCD_DisplayStringLine(174,170,"5");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D5;
		}else if(key == 3){
			LCD_DisplayStringLine(174,170,"10");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D10;
		}else if(key == 4){
			LCD_DisplayStringLine(174,170,"15");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D15;
		}else if(key == 5){
			LCD_DisplayStringLine(174,170,"30");
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetBackColor(LCD_COLOR_BACK);
			LCD_DisplayStringLine(172,230,"Min");
			DIM = D30;
		}
	}
	op_sw = op_off;
	Save_flag();
}
/*触摸屏设置*/
void touch_set(u8 key)
{
	DrawMenu();//重绘菜单栏
	Drawsysmenu();
	LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
	LCD_DrawFullRect(170,210,55,32);
	if(press != 1)
	{
		focus_on1();
	}else{
		focus_off1();
	}
	if(LANG == chs)
	{
		if(key == 1)
		{
			LCD_DisplayStringLine(214,170,"打开");
			TOUCH = op_on;
		}else if(key == 2){
			LCD_DisplayStringLine(214,170,"关闭");
			TOUCH = op_off;
		}
	}else if(LANG == eng){
		if(key == 1)
		{
			LCD_DisplayStringLine(212,170,"ON");
			TOUCH = op_on;
		}else if(key == 2){
			LCD_DisplayStringLine(212,170,"OFF");
			TOUCH = op_off;
		}
	}
	op_sw = op_off;
	Save_flag();
}

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(P1_GPIO_CLK|P2_GPIO_CLK|P3_GPIO_CLK|P4_GPIO_CLK|P5_GPIO_CLK|P6_GPIO_CLK,ENABLE);	/*开启按键GPIO口的时钟*/
	
//	 /* 配置 NVIC */
//	NVIC_Configuration();
	  
	GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/


  
	GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/       
	GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/       
	GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	

	GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/       
	GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/       
	GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P6_PIN;/*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
	GPIO_Init(P6_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
	
//	/* 连接 EXTI 中断源 到P6引脚 */
//	SYSCFG_EXTILineConfig(P6_EXTI_PORTSOURCE,P6_EXTI_PINSOURCE);
//	
//	/* 选择 EXTI 中断源 */
//	EXTI_InitStructure.EXTI_Line = P6_EXTI_LINE;
//	/* 中断模式 */
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	/* 下降沿触发 */
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
//	/* 使能中断/事件线 */
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
}

//扫描键值
void Key_Scan(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;/*设置引脚速率为2MHz */    
	GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/


  
	GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/       
	GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
	GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	

	GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/       
	GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/       
	GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/

	
	GPIO_InitStructure.GPIO_Pin = P6_PIN;/*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
	GPIO_Init(P6_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化          按键*/
	
	GPIO_SetBits(P1_GPIO_PORT,P1_PIN);/*P1脚置高*/
	GPIO_SetBits(P2_GPIO_PORT,P2_PIN);/*P2脚置高*/
	GPIO_SetBits(P3_GPIO_PORT,P3_PIN);/*P3脚置高*/
	GPIO_SetBits(P4_GPIO_PORT,P4_PIN);/*P4脚置高*/
	GPIO_SetBits(P5_GPIO_PORT,P5_PIN);/*P5脚置高*/
//	Key_Delay(0XFFFF);
	if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
	{
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			
//			Key_Delay(0x9FFFF);
			if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 1;//ESC
				}
			}
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 2;//ENTER
				}
			}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 3;//RIGHT
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 4;
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 5;//FUNC_5
				}
			}
		}
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P2_GPIO_PORT,P2_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
				
		Key_Delay(0xFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 6;//FUNC_4
				}
			}			
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 7;//0
				}
				}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 8;//DOWN
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 9;//dot
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 10;//BACKSPACE
				}
			}
		}
				
			
			
			
			
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P3_GPIO_PORT,P3_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 11;//FUNC_3
				}
			}
			else if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 12;//3
				}
			}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 13;//BACKUP
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 14;//9
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 15;//6
				}
			}
		}
			
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P4_GPIO_PORT,P4_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
			
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 16;//FUNC_2
				}
			}
			else if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 17;//2
				}
			}
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 18;//5
				}
			}
			else if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P5_GPIO_PORT,P5_PIN))
				{
					key_value = 19;//8
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 20;//UP
				}
			}
		}
			
		GPIO_InitStructure.GPIO_Pin = P5_PIN;/*选择按键的引脚*/     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;/*设置引脚为输出模式*/   
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;/*设置引脚的输出类型为推挽输出*/    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*设置引脚为上拉模式*/	  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;/*设置引脚速率为2MHz */    
		GPIO_Init(P5_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_SetBits(P5_GPIO_PORT,P5_PIN);/*P2脚置高*/
		GPIO_InitStructure.GPIO_Pin = P1_PIN;/*选择按键的引脚*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;/*设置引脚为输入模式*/   
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;/*设置引脚下拉*/
		GPIO_Init(P1_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P2_PIN;/*选择按键的引脚*/
		GPIO_Init(P2_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P3_PIN;/*选择按键的引脚*/
		GPIO_Init(P3_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		GPIO_InitStructure.GPIO_Pin = P4_PIN;/*选择按键的引脚*/
		GPIO_Init(P4_GPIO_PORT, &GPIO_InitStructure);/*使用上面的结构体初始化按键*/
		Key_Delay(0XFF);
		if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
		{
			if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P1_GPIO_PORT,P1_PIN))
				{
					key_value = 21;//FUNC_1
				}
			}
			else if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P2_GPIO_PORT,P2_PIN))
				{
					key_value = 22;//1
				}
			}
			else if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P3_GPIO_PORT,P3_PIN))
				{
					key_value = 23;//4
				}
			}
			else if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P4_GPIO_PORT,P4_PIN))
				{
					key_value = 24;//LEFT
				}
			}
			else if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
			{
//				Key_Delay(0x9FFFF);
				if(GPIO_ReadInputDataBit(P6_GPIO_PORT,P6_PIN))
				{
					key_value = 25;//7
				}
			}
		}

	}else{
		key_value = 0XFF;
	}
	
}

//按键功能
void Key_Function(void)
{	
	static u8 key_old;
	static u8 key_new;
	key_new = key_value;
	if(key_value != 0XFF)
	{		
		if(key_old != key_new && keystat == 0)
		{
			ButtonSound();//按键音
			if(KEYLOCK==0)
			{
			
				switch(key_value)
			{
				case KEY_1:
				{
					KEY1_HANDLE();
				}
				break;
				case KEY_2:
				{
					KEY2_HANDLE();
				}
				break;
				case KEY_3:
				{
					KEY3_HANDLE();
				}
				break;
				case KEY_4:
				{
					KEY4_HANDLE();
				}
				break;
				case KEY_5:
				{
					KEY5_HANDLE();
				}
				break;
				case KEY_6:
				{
					KEY6_HANDLE();
				}
				break;
				case KEY_7:
				{
					KEY7_HANDLE();
				}
				break;
				case KEY_8:
				{
					KEY8_HANDLE();
				}
				break;
				case KEY_9:
				{
					KEY9_HANDLE();
				}
				break;
				case KEY_0:
				{
					KEY0_HANDLE();
				}
				break;
				case KEY_ACC:
				{
					ACC_HANDLE();
				}
				break;
				case KEY_BACK:
				{
					BACK_HANDLE();
				}
				break;
				case KEY_DOT:
				{
					DOT_HANDLE();
				}
				break;
				case KEY_UP:
				{
					UP_HANDLE();
				}
				break;
				case KEY_DOWN:
				{
					DOWN_HANDLE();
				}
				break;
				case KEY_LEFT:
				{
					LEFT_HANDLE();
				}
				break;
				case KEY_RIGHT:
				{
					RIGHT_HANDLE();
				}
				break;
				case KEY_ENTER:
				{
					ENTER_HANDLE();
					count_flag = 1;
				}
				break;
				case KEY_ESC:
				{
					ESC_HANDLE();
				}
				break;
				case KEY_FUNC1:
				{					
					FUNC1_HANDLE();
				}
				break;
				case KEY_FUNC2:
				{
					FUNC2_HANDLE();
				}
				break;
				case KEY_FUNC3:
				{
					FUNC3_HANDLE();
				}
				break;
				case KEY_FUNC4:
				{
					FUNC4_HANDLE();
				}
				break;
				case KEY_FUNC5:
				{
					FUNC5_HANDLE();
				}
				break;
				default:break;
			}
		  }
			else
			{
				switch(key_value)
				{
					case KEY_FUNC5:
					{
						FUNC5_HANDLE();
					}
					break;
					default:break;
				}
			}
			keystat = 1;
			key_old = key_value;
		}			
	}else{
		keystat = 0;
		key_value = 0xff;
		key_old = 0;
	}		
	

}


/*按键功能函数FUNC_1*/
void FUNC1_HANDLE(void)
{
	switch(page_flag)
	{
	  case manus:
	  {
		  if(op_sw == op_on)
		  {
				if(op_flag == set_trigg)
				{
					TRIGGER = 0;
				}else if(op_flag == set_range){
					RANGE = 8;
				}else if(op_flag == set_mode){
					MODE = 0;
				}else if(op_flag == set_spd){
					SPEED = fast;
				}else if(op_flag == set_filter){
					FILTER = ft_on;
				}else if(op_flag == set_beep){
					BEEP = beep_on;
				}
				Disp_main();
				Esc_menu();
				Save_flag();
			}else{
				page_measure(0);
			}
	  }break;
	  case sysset:
	  {
		  if(op_sw == op_on)
		  {
				if(op_flag == set_lang)
				{
					LANG = chs;
					Esc_menu();
				}else if(op_flag == set_date){
					if(date_page == page1)
					{
						plus_year();
					}else{
						plus_day();
					}
				}else if(op_flag == set_time){
					if(time_page == page1)
					{
						plus_hour();							
					}else{
						plus_sec();
					}
				}else if(op_flag == set_brt){
					brt_set(1);
					Esc_menu();
				}else if(op_flag == set_dim){
					dim_set(1);
					Esc_menu();
				}else if(op_flag == set_touch){
					touch_set(1);
					Esc_menu();
				}else if(op_flag == set_baud){
					BAUD = b9600;
					Esc_menu();
				}
				Disp_sys();				
				Save_flag();
			}else{
				page_measure(0);
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{
				case set_min0:
					DrawLetmenu(1);
					memset(newname, '\0', sizeof(newname));					
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					LCD_DrawFullRect(250,3,160,31);
					op_flag = set_let1;
					break;
				case set_let1:
					DrawLetmenu(11);
					op_flag = set_let11;
					break;
				case set_let2:
					DrawLetmenu(21);
					op_flag = set_let21;
					break;
				case set_let11:
					input_num("A");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let12:
					input_num("E");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let13:
					input_num("I");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let14:
					input_num("M");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let21:
					input_num("Q");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let22:
					input_num("U");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let23:
					input_num("Y");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;	
			}
		}break;
		case setdis:
		{
			switch(op_flag)
			{
				case set_rel:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
					//	Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,50,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(54,210,"打开");
						}else{
							LCD_DisplayStringLine(50,210,"ON");
						}
						REL = 1;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_cal:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
					//	Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,90,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(214,210,"打开");
						}else{
							LCD_DisplayStringLine(210,210,"ON");
						}
						CALCULATION = 1;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				default:
				{
					page_measure(0);
				}break;
			}				
		}break;		
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,50,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"打开");
						}else{
							LCD_DisplayStringLine(50,150,"ON");
						}
						FILTER = ft_on;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_beep:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,90,48,31);
						
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"打开");
						}else{
							LCD_DisplayStringLine(90,150,"ON");
						}
						BEEP = beep_on;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_unit:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,127,130,31);
						LCD_DrawFullRect(255,170,50,31);	
						LCD_DrawFullRect(605,170,50,31);
						LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);

						
						if(LANG == chs)
						{
							LCD_DrawFullRect(150,127,96,31);
							LCD_SetTextColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(130,150,"绝对值爵");
						}
						else{
							LCD_DrawFullRect(150,127,64,31);
							LCD_SetTextColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(127,150,"ABS~");
						}
	//					DISP_CNL_S(127,150,"o");
	//					LCD_DisplayStringLine(127,155,"C");
						focus_off1();
						if(LOWERNOM== 0)
						{
							LCD_DisplayStringLine(170,255,"m#");
						}else if(LOWERNOM == 1){
							LCD_DisplayStringLine(170,255,"#");
						}else if(LOWERNOM == 2){
							LCD_DisplayStringLine(170,255,"k#");
						}else{
							LCD_DisplayStringLine(170,255,"M#");
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
						UNIT = C;
						op_sw = op_off;
						Save_flag();
						DrawUnit();
					}else{
						page_measure(0);
					}
				}break;
				
				case set_spd:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"快速");
						}else{
							LCD_DisplayStringLine(50,500,"FAST");
						}
						SPEED = fast;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_nom:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,90,48,31);
						focus_on1();
						//LCD_DisplayStringLine(87,500,"9600");
						LCD_DisplayStringLine(90,605,"m#");
						//BAUD = b9600;
						NOM=0;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_lower:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(255,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,255,"m#");

						LOWERNOM=0;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_upper:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,605,"m#");

						UPPERNOM=0;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				default:
				{
					page_measure(0);
				}break;
			}				
		}break;
		case separation:
		{
			
		}break;
		case sysinfo:
		{
			page_measure(0);
		}break;
		case graph:
		{
			
		}break;
		case graphset:
		{
			
		}break;
		case history:
		{
			
		}break;
		case calibrate:
		{
			
		}break;
	}
}

/*按键功能函数FUNC_2*/
void FUNC2_HANDLE(void)
{
	switch(page_flag)
	{
		
		case sysset:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_lang)
				{
					LANG = eng;
					Esc_menu();
				}else if(op_flag == set_date){
					if(date_page == page1)
					{
						minus_year();
					}else{
						minus_day();
					}
				}else if(op_flag == set_time){
					if(time_page == page1)
					{
						minus_hour();
					}else{
						minus_sec();
					}
				}else if(op_flag == set_brt){
					brt_set(2);
					Esc_menu();
				}else if(op_flag == set_dim){
					dim_set(2);
					Esc_menu();
				}else if(op_flag == set_touch){
					touch_set(2);
					Esc_menu();
				}else if(op_flag == set_baud){
					BAUD = b19200;
					Esc_menu();
				}
				Disp_sys();				
				Save_flag();
			}else{
				page_setdis();
			}
		}break;
		case setdis:
		{
			switch(op_flag)
			{
				case set_rel:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
					//	Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,50,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(54,210,"关闭");
						}else{
							LCD_DisplayStringLine(50,210,"OFF");
						}
						REL = 0;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_cal:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
					//	Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,90,48,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(214,210,"关闭");
						}else{
							LCD_DisplayStringLine(210,210,"OFF");
						}
						CALCULATION = 0;
						op_sw = op_off;
						Save_flag();
					}else{
						page_sys();
					}
				}break;
				default:
				{
					page_sys();
				}break;
			}				
		}break;	
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,150,"关闭");
						}else{
							LCD_DisplayStringLine(50,150,"OFF");
						}
						FILTER = ft_off;
						op_sw = op_off;
						Save_flag();
					}else{
						page_setdis();
					}
				}break;
				case set_beep:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(90,150,"关闭");
						}else{
							LCD_DisplayStringLine(90,150,"OFF");
						}
						BEEP = beep_off;
						op_sw = op_off;
						Save_flag();
					}else{
						page_setdis();
					}
				}break;
				case set_unit:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,127,144,31);
						LCD_DrawFullRect(255,170,50,31);	
						LCD_DrawFullRect(605,170,50,31);
						LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
									
						//LCD_DisplayStringLine(127,155,"K");
						if(LANG == chs)
						{
						LCD_DrawFullRect(150,127,96,31);
						LCD_SetTextColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(130,150,"百分比粉");
						}
						else{
							LCD_DrawFullRect(150,127,64,31);
							LCD_SetTextColor(LCD_COLOR_BACK);
							LCD_DisplayStringLine(127,150,"PRE%");
						}
						
						//LCD_SetTextColor(LCD_COLOR_YELLOW);
						focus_off1();
						LCD_DisplayStringLine(170,255,"%");
						LCD_DisplayStringLine(170,605,"%");
						UNIT = K;
						op_sw = op_off;
						Save_flag();
						DrawUnit();
					}else{
						page_setdis();
					}
				}break;
				
				case set_spd:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(500,50,64,31);
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"中速");
						}else{
							LCD_DisplayStringLine(50,500,"MED");
						}
						SPEED = moderate;
						op_sw = op_off;
						Save_flag();
					}else{
						page_setdis();
					}
				}break;
				case set_nom:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,90,48,31);
						focus_on1();
						//LCD_DisplayStringLine(87,500,"9600");
						LCD_DisplayStringLine(90,605,"#");
						//BAUD = b9600;
						NOM=1;
						op_sw = op_off;
						Save_flag();
					}else{
						page_setdis();
					}
				}break;
				case set_lower:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(255,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,255,"#");

						LOWERNOM=1;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_upper:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,605,"#");

						UPPERNOM=1;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				default:
				{
					page_setdis();
				}break;
			}				
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{

				case set_let1:
					DrawLetmenu(12);
					op_flag = set_let12;
					break;
				case set_let2:
					DrawLetmenu(22);
					op_flag = set_let22;
					break;
				case set_let11:
					input_num("B");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let12:
					input_num("F");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let13:
					input_num("J");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let14:
					input_num("N");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let21:
					input_num("R");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let22:
					input_num("V");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let23:
					input_num("Z");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;	
			}
		}break;
		case manus:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_trigg)
				{
					TRIGGER = 1;
				}else if(op_flag == set_range){
					RANGE = RANGE;
				}else if(op_flag == set_mode){
					MODE = 0;
				}else if(op_flag == set_spd){
					SPEED = middle;
				}else if(op_flag == set_filter){
					FILTER = ft_off;
				}else if(op_flag == set_beep){
					BEEP = beep_off;
				}
				Disp_main();
				Esc_menu();
				Save_flag();
			}else{
				page_sys();
			}
		}break;
		case sysinfo:
		{
			page_sys();
		}break;
		case graph:
		{
			
		}break;
		case graphset:
		{
			
		}break;
		case separation:
		{
			
		}break;
		case history:
		{
			
		}break;
		case calibrate:
		{
			
		}break;
	}
}

/*按键功能函数FUNC_3*/
void FUNC3_HANDLE(void)
{
	switch(page_flag)
	{
		case manus:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_range){
					RANGE = 9;
				}else if(op_flag == set_spd){
					SPEED = slow;					
				}
				Esc_menu();
				Disp_main();				
				Save_flag();
			}else{
				page_sys();
			}
		}break;
		case sysset:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_date){
					if(date_page == page1)
					{
						plus_mon();
					}
				}else if(op_flag == set_time){
					if(time_page == page1)
					{
						plus_min();
					}
				}else if(op_flag == set_brt){
					brt_set(3);
					Esc_menu();
				}else if(op_flag == set_dim){
					dim_set(3);
					Esc_menu();
				}else if(op_flag == set_baud){
					BAUD = b38400;
					Esc_menu();
				}
				Disp_sys();				
				Save_flag();
			}else{
				page_usb();
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{

				case set_let1:
					DrawLetmenu(13);
					op_flag = set_let13;
					break;
				case set_let2:
					DrawLetmenu(23);
					op_flag = set_let23;
					break;
				case set_let11:
					input_num("C");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let12:
					input_num("G");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let13:
					input_num("K");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let14:
					input_num("O");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let21:
					input_num("S");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let22:
					input_num("W");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_unit:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(150,127,120,31);
						LCD_DrawFullRect(255,170,50,31);							
						LCD_DrawFullRect(605,170,50,31);
						LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
										
						//DISP_CNL_S(127,150,"o");
						//LCD_DisplayStringLine(127,155,"F");
						if(LANG == chs)
						{
							LCD_DrawFullRect(150,127,120,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);	
							LCD_DisplayStringLine(130,150,"直读值");
							LCD_DisplayStringLine(127,225,"SEQ");
						}
						else{
							LCD_DrawFullRect(150,127,80,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);	
							LCD_DisplayStringLine(127,150,"D-SEQ");
						}
						UNIT = F;
						op_sw = op_off;
						Save_flag();
						DrawUnit();
					}else{
					//	page_setdis();
					    page_usb();
					}
				}break;
				
				case set_spd:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						if(LANG == chs)
						{
							LCD_DisplayStringLine(50,500,"慢速");
						}else{
							LCD_DisplayStringLine(50,500,"SLOW");
						}
						SPEED = slow;
						op_sw = op_off;
						Save_flag();
					}else{
						page_usb();
					}
				}break;
				case set_nom:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,90,48,31);
						focus_on1();
						LCD_DisplayStringLine(90,605,"k#");
						NOM = 2;
						op_sw = op_off;
						Save_flag();
					}else{
						page_usb();
					}
				}break;
			  case set_lower:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(255,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,255,"k#");

						LOWERNOM=2;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_upper:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,605,"k#");

						UPPERNOM=2;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				default:
				{
					page_usb();
				}break;
			}
		}break;
		case separation:
		{
			page_sys();
		}break;
		case sysinfo:
		{
			page_usb();
		}break;
		case graph:
		{
			page_sys();
		}break;
		case graphset:
		{
			page_sys();
		}break;
		case history:
		{
			page_sys();
		}break;
		case calibrate:
		{
			page_sys();
		}break;
	}
}

/*按键功能函数FUNC_4*/
void FUNC4_HANDLE(void)
{
	u8 u8range=0;

	switch(page_flag)
	{
		case sysset:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_date){
					if(date_page == page1)
					{
						minus_mon();
					}
				}else if(op_flag == set_time){
					if(time_page == page1)
					{
						minus_min();
					}
				}else if(op_flag == set_brt){
					brt_set(4);
					Esc_menu();
				}else if(op_flag == set_dim){
					dim_set(4);
					Esc_menu();
				}else if(op_flag == set_baud){
					BAUD = b57600;
					Esc_menu();
				}
				Disp_sys();				
				Save_flag();
			}else{
				page_set();
			}
			
		}break;
		case manus:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_range){
					if(RANGE++ == 7)
					{
						RANGE = 0;
					}
				}
				Disp_main();				
				Save_flag();
			}else{
				page_sys();
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{

				case set_let1:
					DrawLetmenu(13);
					op_flag = set_let14;
					break;
				case set_let2:
					DrawLetmenu(23);
					op_flag = set_let24;
					break;
				case set_let11:
					input_num("D");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let12:
					input_num("H");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let13:
					input_num("L");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let14:
					input_num("P");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let21:
					input_num("T");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				case set_let22:
					input_num("X");
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				default:
				{
					page_measure(0);
				}break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_nom:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,90,48,31);
						focus_on1();
						LCD_DisplayStringLine(90,605,"M#");
						NOM = 3;
						op_sw = op_off;
						Save_flag();
					}else{
					//	page_set();
					}
				}break;
				case set_lower:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(255,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,255,"M#");

						LOWERNOM=3;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				case set_upper:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
						LCD_DrawFullRect(605,170,48,31);
						focus_on1();
						LCD_DisplayStringLine(170,605,"M#");

						UPPERNOM=3;
						op_sw = op_off;
						Save_flag();
					}else{
						page_measure(0);
					}
				}break;
				default:
				{
				//	page_cal();
				}break;
			}
		}break;
		case separation:
		{
			page_set();
		}break;
		case sysinfo:
		{
			page_set();
		}break;
		case graph:
		{
			page_set();
		}break;
		case graphset:
		{
			page_set();
		}break;
		case history:
		{
			page_set();
		}break;
		case calibrate:
		{
			page_set();
		}break;
		default:break;
	}
}

/*按键功能函数FUNC_5*/
void FUNC5_HANDLE(void)
{
//	static u16 i;
	u8 u8range=0;
	switch(page_flag)
	{
		case sysset:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_date){
					if(date_page == page1)
					{
						DrawMenu();
						Drawdate2();
						date_page = page2;
					}else{
						DrawMenu();
						Drawdate();
						date_page = page1;
					}
				}else if(op_flag == set_time){
					if(time_page == page1)
					{
						DrawMenu();
						Drawtime2();
						time_page = page2;
					}else{
						DrawMenu();
						Drawtime();
						time_page = page1;
					}
				}else if(op_flag == set_brt){
					brt_set(5);
					Esc_menu();
				}else if(op_flag == set_dim){
					dim_set(5);
					Esc_menu();
				}else if(op_flag == set_baud){
					BAUD = b115200;
					Esc_menu();
				}
				Disp_sys();				
				Save_flag();
			}else{
				page_sysinfo();
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{

				case set_let1:
					DrawLetmenu(2);
					op_flag = set_let2;
					break;
				case set_let2:
					DrawLetmenu(1);
					op_flag = set_let1;
					break;
				default:
				{
					page_sys();
				}break;
			}
		}break;
		case settings:
		{
			switch(op_flag)
			{
/*
				case set_baud:
				{
					if(op_sw == op_on)
					{
						DrawMenu();//重绘菜单栏
						Drawsetmenu();
						focus_on1();
						LCD_DisplayStringLine(87,500,"115200");
						BAUD = b115200;
						op_sw = op_off;
						Save_flag();
					}else{
						page_sep();
					}
				}break;
				*/
				default:
				{
					if(op_sw == op_off)
					{
					//	page_sep();
						page_sysinfo();
					}
				}break;
			}
		}break;
		
		case manus:
		{
			if(op_sw == op_on)
			{
				if(op_flag == set_range){
					if(RANGE-- == 0)
					{
						RANGE = 7;
					}
				}
				Disp_main();				
				Save_flag();
			}else{
				if(KEYLOCK==0)
				{
					KEYLOCK = 1;
					Drawhomemenu();
				}else{
					KEYLOCK = 0;
					Drawhomemenu();
				}
			}
		}break;
		case sysinfo:
		{
			page_sysinfo();
		}break;
		case graph:
		{
			
		}break;
		case graphset:
		{
			
		}break;

	}
}

/*按键功能函数ENTER*/
void ENTER_HANDLE(void)
{
//	u8 u8range=0;

	switch(page_flag)
	{
		
		case settings:
		{
			char dest1[6];
			switch(op_flag)
			{
				case set_filter:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"打开");
						LCD_DisplayStringLine(445,157,"关闭");
					}else{
						LCD_DisplayStringLine(445,35,"ON");
						LCD_DisplayStringLine(445,157,"OFF");
					}
					op_sw = op_on;
				}break;
				case set_beep:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"打开");
						LCD_DisplayStringLine(445,157,"关闭");
					}else{
						LCD_DisplayStringLine(445,35,"ON");
						LCD_DisplayStringLine(445,157,"OFF");
					}
					op_sw = op_on;
				}break;
				case set_unit:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
				if(LANG == chs)
				{
				//	DISP_CNL_S(445,48,"o");
					LCD_DisplayStringLine(445,10,"绝对值爵");
					LCD_DisplayStringLine(445,140,"百分比粉");
				//	DISP_CNL_S(445,298,"o");
					LCD_DisplayStringLine(445,283,"直读值");
				}else{
					LCD_DisplayStringLine(445,20,"ABS~");
					LCD_DisplayStringLine(445,150,"PRE%");
				//	DISP_CNL_S(445,298,"o");
					LCD_DisplayStringLine(445,280,"D-SEQ");
				}
					op_sw = op_on;
				}break;
				
				case set_spd:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					if(LANG == chs)
					{
						LCD_DisplayStringLine(445,35,"快速");
						LCD_DisplayStringLine(445,157,"中速");
						LCD_DisplayStringLine(445,292,"慢速");
					}else{
						LCD_DisplayStringLine(445,35,"FAST");
						LCD_DisplayStringLine(445,157,"MED");
						LCD_DisplayStringLine(445,292,"SLOW");
					}
					op_sw = op_on;
				}break;
				case set_nom:
				{
					DrawMenu();//重绘菜单栏
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_SetBackColor(LCD_COLOR_BLACK);
					LCD_DisplayStringLine(445,40,"m#");
					LCD_DisplayStringLine(445,180,"#");
					LCD_DisplayStringLine(445,300,"k#");
					LCD_DisplayStringLine(445,425,"M#");
					
					op_sw = op_on;
					if(dot_flag == 0){
								NOMA= atoi(data);					
							}else if(dot_flag != 0){
							memset(dest1, '\0', sizeof(dest1));
							strncpy(dest1,data,dot_flag + 1);
							NOMA= atof(dest1);
						    }
							nomafocus_on(NOMA);
				}break;
				case set_upper:
				    {
					 if(UNIT == C)
					 	{
						 DrawMenu();//重绘菜单栏
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_SetBackColor(LCD_COLOR_BLACK);
						LCD_DisplayStringLine(445,40,"m#");
						LCD_DisplayStringLine(445,180,"#");
						LCD_DisplayStringLine(445,300,"k#");
						LCD_DisplayStringLine(445,425,"M#");
						op_sw = op_on;
					 	}
							if(dot_flag == 0){
								UPPER = atoi(data);					
							}else if(dot_flag != 0){
							memset(dest1, '\0', sizeof(dest1));
							strncpy(dest1,data,dot_flag + 1);
							UPPER= atof(dest1);
						    }
							commaxfocus_on(UPPER);
						}break;
			   case set_lower:
						{
				   if(UNIT == C)
					{
						DrawMenu();//重绘菜单栏
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_SetBackColor(LCD_COLOR_BLACK);
						LCD_DisplayStringLine(445,40,"m#");
						 LCD_DisplayStringLine(445,180,"#");
						 LCD_DisplayStringLine(445,300,"k#");
						 LCD_DisplayStringLine(445,425,"M#");
						 op_sw = op_on;
					}
				  			 
							if(dot_flag == 0){
								LOWER= atoi(data);					
							}else if(dot_flag != 0){
							memset(dest1, '\0', sizeof(dest1));
							strncpy(dest1,data,dot_flag + 1);
							LOWER= atof(dest1);
						    }
							comminfocus_on(LOWER);
						}break;
				/*
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
				*/
			}
		}break;
		case manus:
		{
			char dest1[6];
			Disp_main_button();
		}break;

		case sysset:
		{
			Disp_sysset_button();
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{
				
				case set_let1:
				{
					
					memset(newname, '\0', sizeof(newname));
					strcpy(newname,data);
					
					Newfocus_on(newname);
					
					if(filecur<8)
					{
						strcpy(filename[filecur],newname);
						leftfocus_off(filecur+1,filename[filecur]);
					}
					else
					{
						strcpy(filename[filecur+8],newname);
						rightfocus_off(filecur+1,filename[filecur+8]);
					}
					filecur++;
					op_flag = set_min0;
					Save_flag();
				}break;
			}
		}break;
		case setdis:
		{
		char dest1[6];
		switch(op_flag)
					{
						case set_tc_par:
						{

							if(dot_flag == 0){
								TC_PAR = atoi(data);					
							}else if(dot_flag != 0){
							memset(dest1, '\0', sizeof(dest1));
							strncpy(dest1,data,dot_flag + 1);
							TC_PAR= atof(dest1);
						    }
							tc_parfocus_on(TC_PAR);
						}break;
						case set_tc_tep:
						{

							if(dot_flag == 0){
								TC_TEP= atoi(data);					
							}else if(dot_flag != 0){
							memset(dest1, '\0', sizeof(dest1));
							strncpy(dest1,data,dot_flag + 1);
							TC_TEP= atof(dest1);
						    }
							tc_tepfocus_on(TC_TEP);
						}break;
							
						case set_cons:
						{
							
							if(dot_flag == 0){
								CONSTANT= atoi(data); 				
							}else if(dot_flag != 0){
							memset(dest1, '\0', sizeof(dest1));
							strncpy(dest1,data,dot_flag + 1);
								CONSTANT= atof(dest1);
							}
							consfocus_on(CONSTANT);
						}break;

						
						case set_cal:
						{
							DrawMenu();
							DrawTouch();
							op_sw = op_on;
						}break;
						case set_rel:
						{
							DrawMenu();
							DrawTouch();
							op_sw = op_on;
						}break;
						case set_delay:
						{
							RangeDelay = atoi(data);
							if(RangeDelay  > 2000)
							{
								RangeDelay  = 255;
							}
							delaytimefocus_on(RangeDelay );
							Save_flag();
						}break;
					}
		
		}break;
		case graph:
		{
			
		}break;

		case calibrate:
		{
			u8 k;
			char dest[6];			
			if(input_flag == 1)
			{
				k = cor_page * 8 + (op_flag - 83);
				if(dot_flag == 0){
					Correction[k] = ch_temp[k] - atoi(data);
				}else if(dot_flag != 0){
					memset(dest, '\0', sizeof(dest));
					strncpy(dest,data,dot_flag + 1);
					Correction[k] = ch_temp[k] - atof(dest);
				}
				if(Correction[k] < -2200 || Correction[k] > 2200)
				{
					Correction[k] = 0;
				}
//				else{
//					offset[k] = ch_temp[k] - Correction[k];
//					ch_temp[k]-=offset[k];
//				}
				maxfocus_on(op_flag - 82,Correction[k]);				
			}
			Save_flag();
		}break;
	}
}

/*按键功能函数UP*/
void UP_HANDLE(void)
{
	char buf[10];
	u8 u8range=0; 
	switch(page_flag)
	{
		
		case setdis :
		{
			switch(op_flag)
					{
						case  set_tc_par:
						{
							focus_off1();
							tc_parfocus_off(TC_PAR);
							
							focus_on1();
							if(LANG == chs)
							{
								if(REL == op_on)
								{
									LCD_DisplayStringLine(54,210,"打开");
								}else{
									LCD_DisplayStringLine(54,210,"关闭");
								}
							}else{
								if(REL == op_on)
								{
									LCD_DisplayStringLine(50,210,"ON");
								}else{
									LCD_DisplayStringLine(50,210,"OFF");
								}
							}
							if(LANG == chs)
							{
									DrawInstruction("温度补偿开关");
							}else{
									DrawInstruction("REL ON/OFF");
							}
							op_flag = set_rel;
						}break; 
						case set_tc_tep:
						{
							focus_off1();
							
							tc_tepfocus_off(TC_TEP);
							focus_on1();
							tc_parfocus_on(TC_PAR);
							op_flag = set_tc_par;
						}break; 
						case set_delay:
						{
							focus_off1();
							delaytimefocus_off(RangeDelay);
							
							focus_on1();
							tc_tepfocus_on(TC_TEP);
							op_flag = set_tc_tep;
						}break; 
						case set_cal:
						{
							focus_off1();
							if(LANG == chs)
							{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(214,210,"打开");
								}else{
									LCD_DisplayStringLine(214,210,"关闭");
								}
							}else{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(210,210,"ON");
								}else{
									LCD_DisplayStringLine(210,210,"OFF");
								}
							}
							
							focus_on1();
							delaytimefocus_on(RangeDelay);
							
							if(LANG == chs)
							{
								DrawInstruction("修改延时");
							}else{
								DrawInstruction("Input Range Delay");
							}
							op_flag = set_delay;
						}break; 
						case set_cons:
						{
							focus_off1();
							consfocus_off(CONSTANT);
							focus_on1();
								if(LANG == chs)
							{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(214,210,"打开");
								}else{
									LCD_DisplayStringLine(214,210,"关闭");
								}
							}else{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(210,210,"ON");
								}else{
									LCD_DisplayStringLine(210,210,"OFF");
								}
							}
							if(LANG == chs)
							{
									DrawInstruction("计算开关");
							}else{
									DrawInstruction("Calculation ON/OFF");
							}
							op_flag = set_cal;
						}break; 
				}
		
		}break;
		case manus:
		{
			switch(op_flag)
			{
				case set_range:
				{
					op_flag = set_trigg;
					Disp_main();
				}break;				
				case set_mode:
				{
					op_flag = set_range;	
					Disp_main();
				}break;	
				case set_filter:
				{
					op_flag = set_spd;
					Disp_main();
				}break;				
				case set_beep:
				{
					op_flag = set_filter;	
					Disp_main();
				}break;	
			}
		}break;	
		case settings:
		{
			switch(op_flag)
			{
				case set_beep:
				{
					focus_off1();
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
					
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction("比较器设置");
					}else{
						DrawInstruction("Comparator ON/OFF");
					}
					op_flag = set_filter;	
				}break;
				case set_unit:
				{
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
					LCD_DrawFullRect(150,127,144,31);
					/*
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(127,155,"K");
					}
					*/
					focus_off1();
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
		
						}else if(UNIT ==  K){
							LCD_SetTextColor(LCD_COLOR_YELLOW);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"PRE%");
		
						}else{
							LCD_SetTextColor(LCD_COLOR_YELLOW);
							LCD_DisplayStringLine(127,150,"D-SEQ");
						}
					}
					focus_on1();
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
						DrawInstruction("蜂鸣器设置");
					}else{
						DrawInstruction("Buzzer ON/OFF");
					}
					op_flag = set_beep;
				}break;
//				case set_font:
//				{
//					focus_off1();
//					if(FONT == big){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170,150,"大");
//						}else{
//							LCD_DisplayStringLine(170,150,"L");
//						}
//					}else if(FONT == middle){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170,150,"中");
//						}else{
//							LCD_DisplayStringLine(170,150,"M");
//						}
//					}else if(FONT == small){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170,150,"小");
//						}else{
//							LCD_DisplayStringLine(170,150,"S");
//						}
//					}
//					
//					
////					focus_on1();
//					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//					LCD_DrawFullRect(150,127,24,31);
//					if(UNIT == C){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
////						LCD_SetBackColor(LCD_COLOR_YELLOW);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"C");
//					}else if(UNIT == F){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"F");
//					}else{
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_DisplayStringLine(127,155,"K");
//					}
//					if(LANG == chs)
//					{
//						DrawInstruction("温度单位选择");
//					}else{
//						DrawInstruction("Select temperature unit");
//					}
//					op_flag = set_unit;	
//				}break;				
//				case set_file:
//				{
//					focus_off1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					
////					focus_on1();
//					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
//					LCD_DrawFullRect(150,127,24,31);
//					if(UNIT == C){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
////						LCD_SetBackColor(LCD_COLOR_YELLOW);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"C");
//					}else if(UNIT == F){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"F");
//					}else{
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_DisplayStringLine(127,155,"K");
//					}
//					if(LANG == chs)
//					{
//						DrawInstruction("温度单位选择");
//					}else{
//						DrawInstruction("Select temperature unit");
//					}
//					op_flag = set_unit;					
//				}break;
				case set_nom:
				{
					focus_off1();
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
					nomafocus_off(NOMA);

					
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction("采样速度设置");
					}else{
						DrawInstruction("Setup sampling rate");
					}
					op_flag = set_spd;
				}break;
				case set_lower:
				{
					focus_off1();
					comminfocus_off(LOWER);

					
					//focus_on1();
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					if(LANG == chs)
					{
						if(UNIT == C){
							LCD_DrawFullRect(150,127,96,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(130,150,"绝对值爵");
						}else if(UNIT == K){
							LCD_DrawFullRect(150,127,96,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(130,150,"百分比粉");
						}else{
							LCD_DrawFullRect(150,127,120,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(130,150,"直读值");
							LCD_DisplayStringLine(127,225,"SEQ");
						}
					}else{
						if(UNIT == C){
							LCD_DrawFullRect(150,127,64,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"ABS~");
						
						}else if(UNIT == K){
							LCD_DrawFullRect(150,127,64,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"PRE%");
						
						}else{
							LCD_DrawFullRect(150,127,80,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(127,150,"D-SEQ");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("比较模式选择");
					}else{
						DrawInstruction("Select sorting mode");
					}
					op_flag = set_unit;	
				}break;
				/*
				case set_timer:
				{
					stimefocus_off(SAVETIME);
//					LCD_DisplayStringLine(317,150,"1");
//					LCD_DisplayStringLine(322,516,"S");
					
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_SetBackColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_BLACK);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_BLACK);
						LCD_DisplayStringLine(127,155,"K");
					}
					if(LANG == chs)
					{
						DrawInstruction("温度单位选择");
					}else{
						DrawInstruction("Select temperature unit");
					}
					op_flag = set_unit;	
				}break;
				*/
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{
				case set_min1:
				{
					if(spt_page == page1)
					{
						leftfocus_off(1,filename[0]);
						Newfocus_on(newname);
					}
					op_flag = set_min0;
				}break;

				case set_min2:
				{
					if(spt_page == page1)
					{
						leftfocus_off(2,filename[1]);
						leftfocus_on(1,filename[0]);
					}
					op_flag = set_min1;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						leftfocus_off(3,filename[2]);
						leftfocus_on(2,filename[1]);
					}
					op_flag = set_min2;
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						leftfocus_off(4,filename[3]);
						leftfocus_on(3,filename[2]);
					}
					op_flag = set_min3;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						leftfocus_off(5,filename[4]);
						leftfocus_on(4,filename[3]);
					}
					op_flag = set_min4;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						leftfocus_off(6,filename[5]);
						leftfocus_on(5,filename[4]);
					}
					op_flag = set_min5;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						leftfocus_off(7,filename[6]);
						leftfocus_on(6,filename[5]);
					}	
		
					op_flag = set_min6;
				}break;
				case set_min8:
				{
					if(spt_page == page1)
					{
						leftfocus_off(8,filename[7]);
						leftfocus_on(7,filename[6]);
					}	
		
					op_flag = set_min7;
				}break;

				case set_max2:
				{
					if(spt_page == page1)
					{
						rightfocus_off(2,filename[9]);
						rightfocus_on(1,filename[8]);
					}
					op_flag = set_max1;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						rightfocus_off(3,filename[10]);
						rightfocus_on(2,filename[9]);
					}
					op_flag = set_max2;
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						rightfocus_off(4,filename[11]);
						rightfocus_on(3,filename[10]);
					}
					op_flag = set_max3;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						rightfocus_off(5,filename[12]);
						rightfocus_on(4,filename[11]);
					}
					op_flag = set_max4;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						rightfocus_off(6,filename[13]);
						rightfocus_on(5,filename[12]);
					}
					op_flag = set_max5;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						rightfocus_off(7,filename[14]);
						rightfocus_on(6,filename[13]);
					}
					op_flag = set_max6;
				}break;
				case set_max8:
				{
					if(spt_page == page1)
					{
						rightfocus_off(8,filename[15]);
						rightfocus_on(7,filename[14]);
					}
					op_flag = set_max7;
				}break;
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_date:
				{				
					op_flag = set_lang;
					Disp_sys();
				}break;
				case set_brt:
				{
					op_flag = set_date;
					Disp_sys();
				}break;
				case set_dim:
				{
					op_flag = set_brt;
					Disp_sys();
				}break;
				case set_touch:
				{
					op_flag = set_dim;
					Disp_sys();
				}break;
				case set_baud:
				{
					op_flag = set_touch;
					Disp_sys();
				}break;
				case set_timer:
				{
					op_flag = set_baud;
					Disp_sys();
				}break;
			}
		}break;
	

	}
}

/*按键功能函数DOWN*/
void DOWN_HANDLE(void)
{
	char buf[10];
	u8 u8range=0; 
	switch(page_flag)
	{
		
		case setdis :
		{
			switch(op_flag)
					{
						case set_rel:
						{
							focus_off1();
							if(LANG == chs)
							{
								if(REL == op_on)
								{
									LCD_DisplayStringLine(54,210,"打开");
								}else{
									LCD_DisplayStringLine(54,210,"关闭");
								}
							}else{
								if(REL == op_on)
								{
									LCD_DisplayStringLine(50,210,"ON");
								}else{
									LCD_DisplayStringLine(50,210,"OFF");
								}
							}
							focus_on1();
							tc_parfocus_on(TC_PAR);
							op_flag = set_tc_par;
						}break; 
						case set_tc_par:
						{
							focus_off1();
							tc_parfocus_off(TC_PAR);
							focus_on1();
							tc_tepfocus_on(TC_TEP);
							op_flag = set_tc_tep;
						}break; 
						case set_tc_tep:
						{
							focus_off1();
							tc_tepfocus_off(TC_TEP);
							focus_on1();
							delaytimefocus_on(RangeDelay);
							op_flag = set_delay;
						}break; 
						case set_delay:
						{
							focus_off1();
							delaytimefocus_off(RangeDelay);
							focus_on1();
							
							if(LANG == chs)
							{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(214,210,"打开");
								}else{
									LCD_DisplayStringLine(214,210,"关闭");
								}
							}else{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(210,210,"ON");
								}else{
									LCD_DisplayStringLine(210,210,"OFF");
								}
							}
							if(LANG == chs)
							{
									DrawInstruction("计算开关");
							}else{
									DrawInstruction("Calculation ON/OFF");
							}
							op_flag = set_cal;
						}break; 
						case set_cal:
						{
							focus_off1();
							if(LANG == chs)
							{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(214,210,"打开");
								}else{
									LCD_DisplayStringLine(214,210,"关闭");
								}
							}else{
								if(CALCULATION == op_on)
								{
									LCD_DisplayStringLine(210,210,"ON");
								}else{
									LCD_DisplayStringLine(210,210,"OFF");
								}
							}
							focus_on1();
							consfocus_on(CONSTANT);
							if(LANG == chs)
							{
									DrawInstruction("电极常数设置");
							}else{
									DrawInstruction("Set Constant");
							}
							op_flag = set_cons;
						}break; 
				}
		
		}break;
		case settings:
		{
			switch(op_flag)
			{
				case set_filter:
				{
					focus_off1();
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
					
					focus_on1();
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
						DrawInstruction("蜂鸣器设置");
					}else{
						DrawInstruction("Buzzer ON/OFF");
					}
					op_flag = set_beep;
				}break;				
				case set_beep:
				{
					focus_off1();
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
					
					LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_YELLOW);
					
//					if(UNIT == C){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"C");
//					}else if(UNIT == F){
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						DISP_CNL_S(127,150,"o");
//						LCD_DisplayStringLine(127,155,"F");
//					}else{
//						LCD_SetTextColor(LCD_COLOR_BLACK);
//						LCD_DisplayStringLine(127,155,"K");
//					}
					if(LANG == chs)
					{
						if(UNIT == C){
							LCD_DrawFullRect(150,127,96,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(130,150,"绝对值爵");
						}else if(UNIT == K){
							LCD_DrawFullRect(150,127,96,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(130,150,"百分比粉");
						}else{
							LCD_DrawFullRect(150,127,120,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(130,150,"直读值");
							LCD_DisplayStringLine(127,225,"SEQ");
						}
					}else{
						if(UNIT == C){
							LCD_DrawFullRect(150,127,64,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"ABS~");
						
						}else if(UNIT == K){
							LCD_DrawFullRect(150,127,64,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"PRE%");
						
						}else{
							LCD_DrawFullRect(150,127,80,31);
							LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(127,150,"D-SEQ");
						}
					}
					if(LANG == chs)
					{
						DrawInstruction("比较模式选择");
					}else{
						DrawInstruction("Select sorting mode");
					}
					op_flag = set_unit;					
				}break;
				case set_unit:
				{
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
					LCD_DrawFullRect(150,127,144,31);
					focus_off1();
					if(LANG == chs)
					{
						if(UNIT == C){
					//		LCD_DrawFullRect(150,127,96,31);
					//		LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(130,150,"绝对值爵");
						}else if(UNIT == K){
					//		LCD_DrawFullRect(150,127,96,31);
					//		LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(130,150,"百分比粉");
						}else{
					//		LCD_DrawFullRect(150,127,120,31);
					//		LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(130,150,"直读值");
							LCD_DisplayStringLine(127,225,"SEQ");
						}
					}else{
						if(UNIT == C){
					//		LCD_DrawFullRect(150,127,64,31);
					//		LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"ABS~");
						
						}else if(UNIT == K){
					//		LCD_DrawFullRect(150,127,64,31);
					//		LCD_SetTextColor(LCD_COLOR_BLACK);
					//		DISP_CNL_S(127,150,"o");
							LCD_DisplayStringLine(127,150,"PRE%");
						
						}else{
						//	LCD_DrawFullRect(150,127,80,31);
						//	LCD_SetTextColor(LCD_COLOR_BLACK);
							LCD_DisplayStringLine(127,150,"D-SEQ");
						}
					}
					focus_on1();
					comminfocus_on(LOWER);
          
					op_flag = set_lower;					
				}break;
				/*
				case set_unit:
				{
					LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
					LCD_DrawFullRect(150,127,24,31);
					if(UNIT == C){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"C");
					}else if(UNIT == F){
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						DISP_CNL_S(127,150,"o");
						LCD_DisplayStringLine(127,155,"F");
					}else{
						LCD_SetTextColor(LCD_COLOR_YELLOW);
						LCD_DisplayStringLine(127,155,"K");
					}
					
//					focus_on1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					op_flag = set_file;
//					if(LANG == chs)
//					{
//						DrawInstruction("文件名的前缀");
//					}else{
//						DrawInstruction("Input file name prefix");
//					}
					stimefocus_on(SAVETIME);
//					LCD_DisplayStringLine(317,150,"1");
//					LCD_DisplayStringLine(322,516,"S");
					if(LANG == chs)
					{
						DrawInstruction("记录间隔时间");
					}else{
						DrawInstruction("Record interval");
					}
					op_flag = set_timer;
				}break;
				*/
//				case set_font:
//				{
//					focus_off1();
//					if(FONT == big){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170,150,"大");
//						}else{
//							LCD_DisplayStringLine(170,150,"L");
//						}
//					}else if(FONT == middle){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170,150,"中");
//						}else{
//							LCD_DisplayStringLine(170,150,"M");
//						}
//					}else if(FONT == small){
//						if(LANG == chs)
//						{
//							LCD_DisplayStringLine(170,150,"小");
//						}else{
//							LCD_DisplayStringLine(170,150,"S");
//						}
//					}
//					
//					
//					focus_on1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					op_flag = set_file;
//					if(LANG == chs)
//					{
//						DrawInstruction("文件名的前缀");
//					}else{
//						DrawInstruction("Input file name prefix");
//					}
//				}break;				
				case set_spd:
				{
					focus_off1();
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
					
					
					nomafocus_on(NOMA);
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction(" 标称设置");
					}else{
						DrawInstruction("Nominal value");
					}
					
					op_flag = set_nom;
				}break;
//				case set_baud:
//				{
//					focus_off1();
//					if(BAUD == b9600)
//					{
//						LCD_DisplayStringLine(87,500,"9600");
//					}else if(BAUD == b19200){
//						LCD_DisplayStringLine(87,500,"19200");
//					}else if(BAUD == b38400){
//						LCD_DisplayStringLine(87,500,"38400");
//					}else if(BAUD == b57600){
//						LCD_DisplayStringLine(87,500,"57600");
//					}else if(BAUD == b115200){
//						LCD_DisplayStringLine(87,500,"115200");
//					}
//					
//					focus_on1();
//					LCD_DisplayStringLine(317,500,"1");
////					LCD_DisplayStringLine(322,516,"S");
//					if(LANG == chs)
//					{
//						DrawInstruction("记录间隔时间");
//					}else{
//						DrawInstruction("Record interval");
//					}
//					op_flag = set_timer;
//				}break;
			}
		}break;
		case manus:
		{
			switch(op_flag)
			{
				case set_trigg:
				{
					op_flag = set_range;
					Disp_main();
				}break;				
				case set_range:
				{
					op_flag = set_mode;	
					Disp_main();
				}break;
				case set_spd:
				{
					op_flag = set_filter;	
					Disp_main();
				}break;
				case set_filter:
				{
					op_flag = set_beep;	
					Disp_main();
				}break;
				
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{
				case set_min0:
				{
					if(spt_page == page1)
					{
						
						Newfocus_off(newname);
						leftfocus_on(1,filename[0]);
					}
					op_flag = set_min1;
				}break;

				case set_min1:
				{
					if(spt_page == page1)
					{
						leftfocus_on(2,filename[1]);
						leftfocus_off(1,filename[0]);
					}
					op_flag = set_min2;
				}break;
				case set_min2:
				{
					if(spt_page == page1)
					{
						leftfocus_off(2,filename[1]);
						leftfocus_on(3,filename[2]);
					}
					op_flag = set_min3;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						leftfocus_off(3,filename[2]);
						leftfocus_on(4,filename[3]);
					}
					op_flag = set_min4;
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						leftfocus_off(4,filename[3]);
						leftfocus_on(5,filename[4]);
					}
					op_flag = set_min5;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						leftfocus_off(5,filename[4]);
						leftfocus_on(6,filename[5]);
					}
					op_flag = set_min6;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						leftfocus_off(6,filename[5]);
						leftfocus_on(7,filename[6]);
					}
					op_flag = set_min7;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						leftfocus_off(7,filename[6]);
						leftfocus_on(8,filename[7]);
					}	
		
					op_flag = set_min8;
				}break;
				case set_max1:
				{
					if(spt_page == page1)
					{
						rightfocus_on(2,filename[9]);
						rightfocus_off(1,filename[8]);
					}
					op_flag = set_max2;
				}break;
				case set_max2:
				{
					if(spt_page == page1)
					{
						rightfocus_off(2,filename[9]);
						rightfocus_on(3,filename[10]);
					}
					op_flag = set_max3;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						rightfocus_off(3,filename[10]);
						rightfocus_on(4,filename[11]);
					}
					op_flag = set_max4;
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						rightfocus_off(4,filename[11]);
						rightfocus_on(5,filename[12]);
					}
					op_flag = set_max5;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						rightfocus_off(5,filename[12]);
						rightfocus_on(6,filename[13]);
					}
					op_flag = set_max6;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						rightfocus_off(6,filename[13]);
						rightfocus_on(7,filename[14]);
					}
					op_flag = set_max7;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						rightfocus_off(7,filename[14]);
						rightfocus_on(8,filename[15]);
					}
					op_flag = set_max8;
				}break;
				
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_lang:
				{
					op_flag = set_date;
					Disp_sys();
				}break;
				case set_date:
				{
					op_flag = set_brt;
					Disp_sys();
				}break;
				case set_brt:
				{
					op_flag = set_dim;
					Disp_sys();
				}break;
				case set_dim:
				{
					op_flag = set_touch;
					Disp_sys();
				}break;
				case set_touch:
				{
					op_flag = set_baud;
					Disp_sys();
				}break;
				case set_baud:
				{
					op_flag = set_timer;
					Disp_sys();
				}break;
			}
		}break;

	}
}

/*按键功能函数RIGHT*/
void RIGHT_HANDLE(void)
{
	switch(page_flag)
	{
		case settings:
		{
			switch(op_flag)
			{
				
				case set_filter:
				{
					focus_off1();
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
					
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction("采样速度设置");
					}else{
						DrawInstruction("Setup sampling rate");
					}
					op_flag = set_spd;
				}break;	
				case set_lower:
				{
					focus_off1();
					comminfocus_off(LOWER);
					
					focus_on1();
					commaxfocus_on(UPPER);
					op_flag = set_upper;					
				}break;
				case set_beep:
				{
					focus_off1();
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
					
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction(" 标称设置");
					}else{
						DrawInstruction("Nominal value");
					}
					nomafocus_on(NOMA);
					op_flag = set_nom;					
				}break;
//				case set_file:
//				{
//					focus_off1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					
//					focus_on1();
//					LCD_DisplayStringLine(317,500,"1");
////					LCD_DisplayStringLine(322,516,"S");
//					if(LANG == chs)
//					{
//						DrawInstruction("记录间隔时间");
//					}else{
//						DrawInstruction("Record interval");
//					}
//					op_flag = set_timer;				
//				}break;
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{
				case set_min1:
				{
					if(spt_page == page1)
					{
						leftfocus_off(1,filename[0]);
						rightfocus_on(1,filename[8]);
					}
					op_flag = set_max1;
				}break;
				case set_min2:
				{
					if(spt_page == page1)
					{
						leftfocus_off(2,filename[1]);
						rightfocus_on(2,filename[9]);
					}
					op_flag = set_max2;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						leftfocus_off(3,filename[2]);
						rightfocus_on(3,filename[10]);
					}
					op_flag = set_max3;					
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						leftfocus_off(4,filename[3]);
						rightfocus_on(4,filename[11]);
					}
					op_flag = set_max4;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						leftfocus_off(5,filename[4]);
						rightfocus_on(5,filename[12]);
					}
					op_flag = set_max5;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						leftfocus_off(6,filename[5]);
						rightfocus_on(6,filename[13]);
					}
					op_flag = set_max6;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						leftfocus_off(7,filename[6]);
						rightfocus_on(7,filename[14]);
					}
					op_flag = set_max7;
				}break;
				case set_min8:
				{
					if(spt_page == page1)
					{
						leftfocus_off(8,filename[7]);
						rightfocus_on(8,filename[15]);
					}
					op_flag = set_max8;
				}break;
			}
		}break;
		case separation:
		{
			switch(op_flag)
			{
				case set_min1:
				{
					if(spt_page == page1)
					{
						minfocus_off(1,CH1MIN);
						maxfocus_on(1,CH1MAX);
					}else if(spt_page == page2){
						minfocus_off(1,CH9MIN);
						maxfocus_on(1,CH9MAX);
					}else if(spt_page == page3){
						minfocus_off(1,CH17MIN);
						maxfocus_on(1,CH17MAX);
					}else if(spt_page == page4){
						minfocus_off(1,CH25MIN);
						maxfocus_on(1,CH25MAX);
					}else if(spt_page == page5){
						minfocus_off(1,CH33MIN);
						maxfocus_on(1,CH33MAX);
					}
					op_flag = set_max1;
				}break;
				case set_min2:
				{
					if(spt_page == page1)
					{
						minfocus_off(2,CH2MIN);
						maxfocus_on(2,CH2MAX);
					}else if(spt_page == page2){
						minfocus_off(2,CH10MIN);
						maxfocus_on(2,CH10MAX);
					}else if(spt_page == page3){
						minfocus_off(2,CH18MIN);
						maxfocus_on(2,CH18MAX);
					}else if(spt_page == page4){
						minfocus_off(2,CH26MIN);
						maxfocus_on(2,CH26MAX);
					}else if(spt_page == page5){
						minfocus_off(2,CH34MIN);
						maxfocus_on(2,CH34MAX);
					}
					op_flag = set_max2;
				}break;
				case set_min3:
				{
					if(spt_page == page1)
					{
						minfocus_off(3,CH3MIN);
						maxfocus_on(3,CH3MAX);
					}else if(spt_page == page2){
						minfocus_off(3,CH11MIN);
						maxfocus_on(3,CH11MAX);
					}else if(spt_page == page3){
						minfocus_off(3,CH19MIN);
						maxfocus_on(3,CH19MAX);
					}else if(spt_page == page4){
						minfocus_off(3,CH27MIN);
						maxfocus_on(3,CH27MAX);
					}else if(spt_page == page5){
						minfocus_off(3,CH35MIN);
						maxfocus_on(3,CH35MAX);
					}
					op_flag = set_max3;					
				}break;
				case set_min4:
				{
					if(spt_page == page1)
					{
						minfocus_off(4,CH4MIN);
						maxfocus_on(4,CH4MAX);
					}else if(spt_page == page2){
						minfocus_off(4,CH12MIN);
						maxfocus_on(4,CH12MAX);
					}else if(spt_page == page3){
						minfocus_off(4,CH20MIN);
						maxfocus_on(4,CH20MAX);
					}else if(spt_page == page4){
						minfocus_off(4,CH28MIN);
						maxfocus_on(4,CH28MAX);
					}else if(spt_page == page5){
						minfocus_off(4,CH36MIN);
						maxfocus_on(4,CH36MAX);
					}
					op_flag = set_max4;
				}break;
				case set_min5:
				{
					if(spt_page == page1)
					{
						minfocus_off(5,CH5MIN);
						maxfocus_on(5,CH5MAX);
					}else if(spt_page == page2){
						minfocus_off(5,CH13MIN);
						maxfocus_on(5,CH13MAX);
					}else if(spt_page == page3){
						minfocus_off(5,CH21MIN);
						maxfocus_on(5,CH21MAX);
					}else if(spt_page == page4){
						minfocus_off(5,CH29MIN);
						maxfocus_on(5,CH29MAX);
					}else if(spt_page == page5){
						minfocus_off(5,CH37MIN);
						maxfocus_on(5,CH37MAX);
					}
					op_flag = set_max5;
				}break;
				case set_min6:
				{
					if(spt_page == page1)
					{
						minfocus_off(6,CH6MIN);
						maxfocus_on(6,CH6MAX);
					}else if(spt_page == page2){
						minfocus_off(6,CH14MIN);
						maxfocus_on(6,CH14MAX);
					}else if(spt_page == page3){
						minfocus_off(6,CH22MIN);
						maxfocus_on(6,CH22MAX);
					}else if(spt_page == page4){
						minfocus_off(6,CH30MIN);
						maxfocus_on(6,CH30MAX);
					}else if(spt_page == page5){
						minfocus_off(6,CH38MIN);
						maxfocus_on(6,CH38MAX);
					}
					op_flag = set_max6;
				}break;
				case set_min7:
				{
					if(spt_page == page1)
					{
						minfocus_off(7,CH7MIN);
						maxfocus_on(7,CH7MAX);
					}else if(spt_page == page2){
						minfocus_off(7,CH15MIN);
						maxfocus_on(7,CH15MAX);
					}else if(spt_page == page3){
						minfocus_off(7,CH23MIN);
						maxfocus_on(7,CH23MAX);
					}else if(spt_page == page4){
						minfocus_off(7,CH31MIN);
						maxfocus_on(7,CH31MAX);
					}else if(spt_page == page5){
						minfocus_off(7,CH39MIN);
						maxfocus_on(7,CH39MAX);
					}
					op_flag = set_max7;
				}break;
				case set_min8:
				{
					if(spt_page == page1)
					{
						minfocus_off(8,CH8MIN);
						maxfocus_on(8,CH8MAX);
					}else if(spt_page == page2){
						minfocus_off(8,CH16MIN);
						maxfocus_on(8,CH16MAX);
					}else if(spt_page == page3){
						minfocus_off(8,CH24MIN);
						maxfocus_on(8,CH24MAX);
					}else if(spt_page == page4){
						minfocus_off(8,CH32MIN);
						maxfocus_on(8,CH32MAX);
					}else if(spt_page == page5){
						minfocus_off(8,CH40MIN);
						maxfocus_on(8,CH40MAX);
					}
					op_flag = set_max8;
				}break;
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_date:
				{
					if(LANG == chs)
					{
						DrawInstruction("时-分-秒");
					}else{
						DrawInstruction("HOUR-MINUTE-SECOND");
					}					
					op_flag = set_time;
				}break;
			}
		}break;
		case manus:
		{
			switch(op_flag)
			{
				case set_trigg:
				{
					op_flag = set_spd;
					Disp_main();
				}break;				
				case set_range:
				{
					op_flag = set_filter;	
					Disp_main();
				}break;
				case set_mode:
				{
					op_flag = set_beep;	
					Disp_main();
				}break;				
			}
		}break;		
	}
}

/*按键功能函数LEFT*/
void LEFT_HANDLE(void)
{
	switch(page_flag)
	{
		case manus:
		{
			switch(op_flag)
			{
				case set_spd:
				{
					op_flag = set_trigg;
					Disp_main();
				}break;				
				case set_filter:
				{
					op_flag = set_range;	
					Disp_main();
				}break;
				case set_beep:
				{
					op_flag = set_mode;	
					Disp_main();
				}break;				
			}
		}break;	
		case settings:
		{
			switch(op_flag)
			{
				case set_spd:
				{
					focus_on1();
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
					if(LANG == chs)
					{
						DrawInstruction("比较器设置");
					}else{
						DrawInstruction("Comparator ON/OFF");
					}
					
					focus_off1();
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
					op_flag = set_filter;
				}break;
				case set_upper:
				{
					focus_off1();
					
					commaxfocus_off(UPPER);
					focus_on1();
					comminfocus_on(LOWER);
					op_flag = set_lower;					
				}break;
				case set_nom:
				{
					focus_on1();
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
						DrawInstruction("蜂鸣器设置");
					}else{
						DrawInstruction("Buzzer ON/OFF");
					}
					
					focus_off1();
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
	                nomafocus_off(NOMA);
					op_flag = set_beep;					
				}break;
//				case set_timer:
//				{
//					focus_on1();
//					LCD_DisplayStringLine(317,150,"AUTO");
//					if(LANG == chs)
//					{
//						DrawInstruction("输入文件名的前缀");
//					}else{
//						DrawInstruction("Input file name prefix");
//					}
//					
//					focus_off1();
//					LCD_DisplayStringLine(317,500,"1");
////					LCD_DisplayStringLine(322,516,"S");
//					op_flag = set_file;				
//				}break;
			}
		}break;
		case usbmanu:
		{
			switch(op_flag)
			{
				case set_max1:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(1,filename[8]);
						leftfocus_on(1,filename[0]);
					}
					op_flag = set_min1;
				}break;
				case set_max2:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(2,filename[9]);
						leftfocus_on(2,filename[1]);
					}
					op_flag = set_min2;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(3,filename[10]);
						leftfocus_on(3,filename[2]);
					}
					op_flag = set_min3;					
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(4,filename[11]);
						leftfocus_on(4,filename[3]);
					}
					op_flag = set_min4;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(5,filename[12]);
						leftfocus_on(5,filename[4]);
					}
					op_flag = set_min5;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(6,filename[13]);
						leftfocus_on(6,filename[5]);
					}
					op_flag = set_min6;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(7,filename[14]);
						leftfocus_on(7,filename[6]);
					}
					op_flag = set_min7;
				}break;
				case set_max8:
				{
					if(spt_page == page1)
					{
						
						rightfocus_off(8,filename[15]);
						leftfocus_on(8,filename[7]);
					}
					op_flag = set_min8;
				}break;
			}
		}break;
		case separation:
		{
			switch(op_flag)
			{
				case set_max1:
				{
					if(spt_page == page1)
					{
						minfocus_on(1,CH1MIN);
						maxfocus_off(1,CH1MAX);
					}else if(spt_page == page2){
						minfocus_on(1,CH9MIN);
						maxfocus_off(1,CH9MAX);
					}else if(spt_page == page3){
						minfocus_on(1,CH17MIN);
						maxfocus_off(1,CH17MAX);
					}else if(spt_page == page4){
						minfocus_on(1,CH25MIN);
						maxfocus_off(1,CH25MAX);
					}else if(spt_page == page5){
						minfocus_on(1,CH33MIN);
						maxfocus_off(1,CH33MAX);
					}
					op_flag = set_min1;
				}break;
				case set_max2:
				{
					if(spt_page == page1)
					{
						minfocus_on(2,CH2MIN);
						maxfocus_off(2,CH2MAX);
					}else if(spt_page == page2){
						minfocus_on(2,CH10MIN);
						maxfocus_off(2,CH10MAX);
					}else if(spt_page == page3){
						minfocus_on(2,CH18MIN);
						maxfocus_off(2,CH18MAX);
					}else if(spt_page == page4){
						minfocus_on(2,CH26MIN);
						maxfocus_off(2,CH26MAX);
					}else if(spt_page == page5){
						minfocus_on(2,CH34MIN);
						maxfocus_off(2,CH34MAX);
					}
					op_flag = set_min2;
				}break;
				case set_max3:
				{
					if(spt_page == page1)
					{
						minfocus_on(3,CH3MIN);
						maxfocus_off(3,CH3MAX);
					}else if(spt_page == page2){
						minfocus_on(3,CH11MIN);
						maxfocus_off(3,CH11MAX);
					}else if(spt_page == page3){
						minfocus_on(3,CH19MIN);
						maxfocus_off(3,CH19MAX);
					}else if(spt_page == page4){
						minfocus_on(3,CH27MIN);
						maxfocus_off(3,CH27MAX);
					}else if(spt_page == page5){
						minfocus_on(3,CH35MIN);
						maxfocus_off(3,CH35MAX);
					}
					op_flag = set_min3;					
				}break;
				case set_max4:
				{
					if(spt_page == page1)
					{
						minfocus_on(4,CH4MIN);
						maxfocus_off(4,CH4MAX);
					}else if(spt_page == page2){
						minfocus_on(4,CH12MIN);
						maxfocus_off(4,CH12MAX);
					}else if(spt_page == page3){
						minfocus_on(4,CH20MIN);
						maxfocus_off(4,CH20MAX);
					}else if(spt_page == page4){
						minfocus_on(4,CH28MIN);
						maxfocus_off(4,CH28MAX);
					}else if(spt_page == page5){
						minfocus_on(4,CH36MIN);
						maxfocus_off(4,CH36MAX);
					}
					op_flag = set_min4;
				}break;
				case set_max5:
				{
					if(spt_page == page1)
					{
						minfocus_on(5,CH5MIN);
						maxfocus_off(5,CH5MAX);
					}else if(spt_page == page2){
						minfocus_on(5,CH13MIN);
						maxfocus_off(5,CH13MAX);
					}else if(spt_page == page3){
						minfocus_on(5,CH21MIN);
						maxfocus_off(5,CH21MAX);
					}else if(spt_page == page4){
						minfocus_on(5,CH29MIN);
						maxfocus_off(5,CH29MAX);
					}else if(spt_page == page5){
						minfocus_on(5,CH37MIN);
						maxfocus_off(5,CH37MAX);
					}
					op_flag = set_min5;
				}break;
				case set_max6:
				{
					if(spt_page == page1)
					{
						minfocus_on(6,CH6MIN);
						maxfocus_off(6,CH6MAX);
					}else if(spt_page == page2){
						minfocus_on(6,CH14MIN);
						maxfocus_off(6,CH14MAX);
					}else if(spt_page == page3){
						minfocus_on(6,CH22MIN);
						maxfocus_off(6,CH22MAX);
					}else if(spt_page == page4){
						minfocus_on(6,CH30MIN);
						maxfocus_off(6,CH30MAX);
					}else if(spt_page == page5){
						minfocus_on(6,CH38MIN);
						maxfocus_off(6,CH38MAX);
					}
					op_flag = set_min6;
				}break;
				case set_max7:
				{
					if(spt_page == page1)
					{
						minfocus_on(7,CH7MIN);
						maxfocus_off(7,CH7MAX);
					}else if(spt_page == page2){
						minfocus_on(7,CH15MIN);
						maxfocus_off(7,CH15MAX);
					}else if(spt_page == page3){
						minfocus_on(7,CH23MIN);
						maxfocus_off(7,CH23MAX);
					}else if(spt_page == page4){
						minfocus_on(7,CH31MIN);
						maxfocus_off(7,CH31MAX);
					}else if(spt_page == page5){
						minfocus_on(7,CH39MIN);
						maxfocus_off(7,CH39MAX);
					}
					op_flag = set_min7;
				}break;
				case set_max8:
				{
					if(spt_page == page1)
					{
						minfocus_on(8,CH8MIN);
						maxfocus_off(8,CH8MAX);
					}else if(spt_page == page2){
						minfocus_on(8,CH16MIN);
						maxfocus_off(8,CH16MAX);
					}else if(spt_page == page3){
						minfocus_on(8,CH24MIN);
						maxfocus_off(8,CH24MAX);
					}else if(spt_page == page4){
						minfocus_on(8,CH32MIN);
						maxfocus_off(8,CH32MAX);
					}else if(spt_page == page5){
						minfocus_on(8,CH40MIN);
						maxfocus_off(8,CH40MAX);
					}
					op_flag = set_min8;
				}break;
			}
		}break;
		case sysset:
		{
			switch(op_flag)
			{
				case set_time:
				{
					if(LANG == chs)
					{
						DrawInstruction("年-月-日");
					}else{
						DrawInstruction("YEAR-MONTH-DAY");
					}
					op_flag = set_date;
				}break;
			}
		}break;
	}
}

void KEY1_HANDLE(void)
{		
	//u8 i;
	switch(page_flag)
	{
		case display:
		{
//			TouchCal();
//			DrawPowOff();
//			LCD_Clear(LCD_COLOR_BLACK);
//			page_flag = 0xff;
			DrawLogo(200,250);
		}break;
		case separation:
		{
			input_num("1");			
		}break;
		case graphset:
		{
			input_num("1");			
		}break;
		case calibrate:
		{
			input_num("1");			
		}break;
		case history:
		{
			input_num("1");			
		}break;
		case touchcal:
		{
			XYCAL(1);
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("1");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("1");	
			}
		}break;
		case usbmanu:
		{
			input_num("1");	
		}break;	
		case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("1");
				}break;
				case set_tc_par:
				{
					input_num("1");
				}break;
				case set_tc_tep:
				{
					input_num("1");
				}break;
				case set_cons:
				{
					input_num("1");
				}break;
			}	
		}break;
		default:
		{
//			for(i=0;i<8;i++)
//			{
//				Usart_SendByte(DEBUG_USART,tempreq[i]);//请求温度数据
//			}
		}break;
	}
}

void KEY2_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("2");
		}break;
		case graphset:
		{
			input_num ("2");
		}break;
		case calibrate:
		{
			input_num("2");			
		}break;
		case history:
		{
			input_num("2");			
		}break;
		case display:
		{
//			BEEP_ON;
//			Delay(0x3ff);
//			BEEP_OFF;
//			 CH376FileClose(TRUE);
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("2");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("2");	
			}
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("2");
				}break;
				case set_tc_par:
				{
					input_num("2");
				}break;
				case set_tc_tep:
				{
					input_num("2");
				}break;
				case set_cons:
				{
					input_num("2");
				}break;
			}	
		}break;
		case usbmanu:
		{
			input_num("2");	
		}break;
		case touchcal:
		{
			XYCAL(2);
		}break;

	}
}

void KEY3_HANDLE(void)
{		
	switch(page_flag)
	{
		case display:
		{
//			BEEP_OFF;
		}break;
		case separation:
		{
			input_num("3");
		}break;
		case graphset:
		{
			input_num("3");
		}break;
		case calibrate:
		{
			input_num("3");			
		}break;
		case history:
		{
			input_num("3");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("3");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("3");	
			}
		}break;
		case usbmanu:
		{
			input_num("3");	
		}break;
				case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("3");
				}break;
				case set_tc_par:
				{
					input_num("3");
				}break;
				case set_tc_tep:
				{
					input_num("3");
				}break;
				case set_cons:
				{
					input_num("3");
				}break;
			}	
		}break;
		case touchcal:
		{
			XYCAL(3);
		}break;
	}
}

void KEY4_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("4");
		}break;
		case graphset:
		{
			input_num("4");
		}break;
		case calibrate:
		{
			input_num("4");			
		}break;
		case history:
		{
			input_num("4");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("4");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("4");	
			}
		}break;
		case usbmanu:
		{
			input_num("4");	
		}break;
				case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("4");
				}break;
				case set_tc_par:
				{
					input_num("4");
				}break;
				case set_tc_tep:
				{
					input_num("4");
				}break;
				case set_cons:
				{
					input_num("4");
				}break;
			}	
		}break;
		case touchcal:
		{
			XYCAL(4);
		}break;
	}
}

void KEY5_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("5");
		}break;
		case graphset:
		{
			input_num("5");
		}break;
		case calibrate:
		{
			input_num("5");			
		}break;
		case history:
		{
			input_num("5");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("5");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("5");	
			}
		}break;
		case usbmanu:
		{
			input_num("5");	
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("5");
				}break;
				case set_tc_par:
				{
					input_num("5");
				}break;
				case set_tc_tep:
				{
					input_num("5");
				}break;
				case set_cons:
				{
					input_num("5");
				}break;
			}	
		}break;
		case touchcal:
		{
			XYCAL(5);
		}break;
	}
}

void KEY6_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("6");
		}break;
		case graphset:
		{
			input_num("6");
		}break;
		case calibrate:
		{
			input_num("6");			
		}break;
		case history:
		{
			input_num("6");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("6");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("6");	
			}
		}break;
		case usbmanu:
		{
			input_num("6");	
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("6");
				}break;
				case set_tc_par:
				{
					input_num("6");
				}break;
				case set_tc_tep:
				{
					input_num("6");
				}break;
				case set_cons:
				{
					input_num("6");
				}break;
			}	
		}break;
		case touchcal:
		{
			XYCAL(6);
		}break;
	}
}

void KEY7_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("7");
		}break;
		case graphset:
		{
			input_num("7");
		}break;
		case calibrate:
		{
			input_num("7");			
		}break;
		case history:
		{
			input_num("7");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("7");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("7");	
			}
		}break;
		case usbmanu:
		{
			input_num("7");	
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
					case set_delay:
				{
					input_num("7");
				}break;
				case set_tc_par:
				{
					input_num("7");
				}break;
				case set_tc_tep:
				{
					input_num("7");
				}break;
				case set_cons:
				{
					input_num("7");
				}break;
			}	
		}break;
		case touchcal:
		{
			XYCAL(7);
		}break;
	}
}

void KEY8_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("8");
		}break;
		case graphset:
		{
			input_num("8");
		}break;
		case calibrate:
		{
			input_num("8");			
		}break;
		case history:
		{
			input_num("8");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("8");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("8");	
			}
		}break;
		case usbmanu:
		{
			input_num("8");	
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
					case set_delay:
				{
					input_num("8");
				}break;
				case set_tc_par:
				{
					input_num("8");
				}break;
				case set_tc_tep:
				{
					input_num("8");
				}break;
				case set_cons:
				{
					input_num("8");
				}break;
			}	
		}break;
		case touchcal:
		{
			XYCAL(8);
		}break;
	}
}

void KEY9_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("9");			
		}break;
		case graphset:
		{
			input_num("9");
		}break;
		case calibrate:
		{
			input_num("9");			
		}break;
		case history:
		{
			input_num("9");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("9");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("9");	
			}
		}break;
		case usbmanu:
		{
			input_num("9");	
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
					case set_delay:
				{
					input_num("9");
				}break;
				case set_tc_par:
				{
					input_num("9");
				}break;
				case set_tc_tep:
				{
					input_num("9");
				}break;
				case set_cons:
				{
					input_num("9");
				}break;
			}	
		}break;
	}
}

void KEY0_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num("0");
		}break;
		case graphset:
		{
			input_num("0");
		}break;
		case calibrate:
		{
			input_num("0");			
		}break;
		case history:
		{
			input_num("0");			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				input_num("0");	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num("0");	
			}
		}break;
		case usbmanu:
		{
			input_num("0");	
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
				case set_delay:
				{
					input_num("0");
				}break;
				case set_tc_par:
				{
					input_num("0");
				}break;
				case set_tc_tep:
				{
					input_num("0");
				}break;
				case set_cons:
				{
					input_num("0");
				}break;
				
				
			}	
		}break;
	}
}

void DOT_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			input_num(".");
		}break;
		case graphset:
		{
			input_num(".");
		}break;
		case calibrate:
		{
			input_num(".");			
		}break;
	  case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				input_num(".");	
			}
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
				case set_tc_par:
				{
					input_num(".");
				}break;
				case set_tc_tep:
				{
					input_num(".");
				}break;
				case set_cons:
				{
					input_num(".");
				}break;
				
				
			}	
		}break;
	}
}

void BACK_HANDLE(void)
{		
	switch(page_flag)
	{
		case separation:
		{
			del_num();
		}break;
		case graphset:
		{
			del_num();
		}break;
		case calibrate:
		{
			del_num();			
		}break;
		case history:
		{
			del_num();			
		}break;
		case sysset:
		{
			if(op_flag == set_timer)
			{
				del_num();	
			}
		}break;
		case settings:
		{
			if(op_flag == set_upper||op_flag == set_lower||op_flag ==set_nom)
			{
				del_num();
			}
		}break;
		case setdis:
		{
			switch(op_flag)
			{					
				case set_tc_par:
				{
					del_num();
				}break;
				case set_tc_tep:
				{
					del_num();
				}break;
				case set_cons:
				{
					del_num();
				}break;
				case set_delay:
				{
					del_num();
				}break;
				
			}	
		}break;
	}
}
void ESC_HANDLE(void)
{
	switch(page_flag)
	{
		case display:
		{
			if(op_flag == type_1 || op_flag == type_2 || op_flag == type_3)
			{
				op_flag = home_type;
				DrawMenu();
				Drawhomemenu();
			}
		}break;
		case sysset:
		{
			DrawMenu();
			Drawsysmenu();
			op_sw = op_off;
		}break;
		case settings:
		{
			DrawMenu();
			Drawsetmenu();
			op_sw = op_off;
		}break;
		case setdis:
		{
			DrawMenu();
			Drawset_dismenu();
			op_sw = op_off;
		}break;
		case manus:
		{
			Esc_menu();
		}break;
        case usbmanu:
		{
			
			if(op_flag>=set_let1&&op_flag<=set_let24)
			{
				DrawMenu();
				memset(newname, '\0', sizeof(newname));				
				Newfocus_on(newname);
				op_flag=set_min0;
			}
			else
			{

			}
			op_sw = op_off;
		}break;
		case calibrate:
		{
			
		}break;
	}
}
void ACC_HANDLE(void)
{
//	u8 i;
	
	//	SPI_Flash_PowerDown();
	//		SPI_Flash_WAKEUP();
	//	for(i=1;i<11;i++)
	//	{
	//		SPI_FLASH_SectorErase(i);
	//	}
	//	Save_Sflag();
	switch(page_flag)
	{
		case calibrate:
		{
			u8 k;
			k = cor_page * 8 + (op_flag - 83);
			Correction[k] = 0;
			maxfocus_on(op_flag - 82,Correction[k]);
			Save_flag();
		}break;
		default:Erase_all();
	}
}


void Utest(void)
{
	u8 buf[200];
	u8 i;
	static u8 filename1[64];
	static u8 TarName[64];
	static u8 s;
	
	static u32 udcount;
	if(fileflag == 0)
	{
		fileflag = 1;
		udcount = 0;
		sprintf((char *)filename1, "/%02d%02d%02d%02d.XLS",MONTH
											,DATE
											,HOURS
											,MINUTES); /* 目标文件名 */
		strcpy((char *)TarName,filename1);
//		s = CH376FileCreatePath("\\TEST.XLS");
		s = CH376FileCreatePath(TarName);
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		
		switch(TCTYPE)
		{
			case TCT:
			{
				sprintf((char *)buf,"热电偶类型:T-%d",TCT);
			}break;
			case TCK:
			{
				sprintf((char *)buf,"热电偶类型:K-%d",TCK);
			}break;
			case TCJ:
			{
				sprintf((char *)buf,"热电偶类型:J-%d",TCJ);
			}break;
			case TCN:
			{
				sprintf((char *)buf,"热电偶类型:N-%d",TCN);
			}break;
			case TCE:
			{
				sprintf((char *)buf,"热电偶类型:E-%d",TCE);
			}break;
			case TCS:
			{
				sprintf((char *)buf,"热电偶类型:S-%d",TCS);
			}break;
			case TCR:
			{
				sprintf((char *)buf,"热电偶类型:R-%d",TCR);
			}break;
			case TCB:
			{
				sprintf((char *)buf,"热电偶类型:B-%d",TCB);
			}break;
			case PT100:
			{
				sprintf((char *)buf,"热电偶类型:PT100-%d",PT100);
			}break;
		}
		s = CH376ByteWrite( buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		switch(UNIT)
		{
			case C:
			{
				sprintf((char *)buf,"\t单位:℃-%d\n",C);
			}break;
			case F:
			{
				sprintf((char *)buf,"\t单位:℉-%d\n",F);
			}break;
			case K:
			{
				sprintf((char *)buf,"\t单位:K-%d\n",K);
			}break;
		}
		s = CH376ByteWrite(buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		CH376ByteLocate(0xFFFFFFFF);
		sprintf((char *)buf,"\t日期\t时间\t通道1\t通道2\t通道3\t通道4\t通道5\t通道6\t通道7\t通道8\t通道9\t通道10\t通道11\t通道12\t通道13\t通道14\t通道15\t通道16");
		s = CH376ByteWrite(buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
		s = CH376FileClose(TRUE);
		if(s != 0x14)
		{
			//fileflag = 0;
		}
	}
	Delay(100);
//	s = CH376FileOpenPath("\\TEST.XLS");
	strcpy((char *)TarName,filename1);
	s = CH376FileOpenPath(TarName);
	if(s != 0x14)
	{
		//fileflag = 0;
	}
	s = CH376ByteLocate(0xFFFFFFFF);
	if(s != 0x14)
	{
		//fileflag = 0;
	}
	//////// 写入
	udcount++;
	sprintf(buf, "\n%d\t20%02d-%02d-%02d\t%02d:%02d:%02d"
											,udcount
											,YEAR
											,MONTH
											,DATE
											,HOURS
											,MINUTES
											,SECONDS
											); /* 目标文件名 */
	s = CH376ByteWrite( buf, strlen((const char *)buf), NULL ); 
	if(s != 0x14)
	{
		//fileflag = 0;
	}
	for(i=0;i<16;i++)
	{

		if(ch_temp[i] > 1999)
		{
			strcpy(buf,"\tN/A");
		}else{
			sprintf(buf,"\t%.1f",ch_temp[i]);
		}
		s = CH376ByteWrite( buf, strlen((const char *)buf), NULL );
		if(s != 0x14)
		{
			//fileflag = 0;
		}
	}
//	strcpy((char *)buf, "Test ,Hello World!" );
	
	
	//printf("s=%02x \n",(unsigned short)s );
	s = CH376FileClose(TRUE);
	if(s != 0x14)
	{
		//fileflag = 0;
	}
}

//按键声音
void ButtonSound(void)
{
	BEEP_ON;
	Delay(0x5ff);
	BEEP_OFF;
}

/*********************************************END OF FILE**********************/
