/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDC液晶显示汉字(显示任意大小)
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./key/bsp_key.h"
#include "./beep/bsp_beep.h"
#include "./tim/bsp_basic_tim.h"
#include "./flash/bsp_spi_flash.h"
#include "./systick/bsp_SysTick.h"
#include "./RTC/bsp_rtc.h"
#include "jk508.h"
#include  "usbd_hid_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"
#include "./internalFlash/bsp_internalFlash.h"
#include "./ch376/ch376.h"
#include "touchscreen.h"
#include "./FILESYS/FILESYS.h"
#include <string.h>

/** @defgroup APP_HID_Private_Variables
  * @{
  */ 



void UARTRECHANDLE(void);
u8 usbbuf[0x40];
u8 usbsendbuf[0x40];
u8 savedata[80];
u8 uinfo[64];
//u8 usbreadtime = 0;
u8 eqmtstatus;
u16 crcwatch;
uint16_t readcrc;
u8 *crec;
u8 *csend;
u16 datasize;
u8 usbstatus = UNKNOWN;
u16 watch;
u16 ureadcrc;
u8 *ucrc;
extern u8 uartflag;
//u8 p1,p2,p3,p4,p5,p6,p7,p8;


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;


union 
{
   unsigned char FLAG_VAL1;
		
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

union 
{
   unsigned char FLAG_VAL2;
		
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

union 
{
   unsigned char FLAG_VAL3;
		
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

union 
{
   unsigned char FLAG_VAL4;

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

union 
{
   unsigned char FLAG_VAL5;
		
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

union 
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

union 
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

union 
{
   unsigned char FLAG_VAL8;
	
   struct 
   {
       unsigned char  BR:3;    //字体
       unsigned char  SP:3;    //速度
       
   }BIT_FLAG;
}FLAG8;

typedef __packed struct {
    u8 uCmd;
    u8 Size;
    u32 u32Arg1;
    u32 u32Arg2;
    u32 u32Signature;
    u32 u32Checksum;
}CMD_T;

//读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;


void Delay(__IO u32 nCount); 
void LCD_Test(void);
void Printf_Charater(void);
int i;
u16 count = 0;

float ch_temp[40];

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

int main(void)
		{
	static u8 powerstat;
//	uint8_t  buf[0x40];
//	static u8 ledstat;
//	u8 test[9] = {0X01,0X03,0X02,0X58,0X00,0X01,0X02,0X00,0X05};
	 __IO uint32_t i = 0;
//	static u8 reqtempcount;
//	static u8 ucount;
	static u8 urecount;
	static u16 usavecount;
	static u8 dispflag=0;
	static u32 dispcnt=0,xx;
	u32 j = 0;
			u8 strsum[8]="000.000";
	KEYLOCK = 0;

		/*初始化SDRAM模块*/
	SDRAM_Init();
	 /*初始化液晶屏*/
    LCD_Init();

    LCD_LayerInit();
    LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_Clear(LCD_COLOR_BLACK);
	
	SysTick_Init();
	Delay(500);
	/* LED 端口初始化 */
//	LED_GPIO_Config();	 
	power_on();
	/* 16M串行flash W25Q128初始化 */
	SPI_FLASH_Init();
	
		/*串口初始化*/
//	Debug_USART_Config();
	 
	/* 获取 Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	Touch_GPIO_Config();
	Delay(200);

	/*按键初始化*/
	Key_GPIO_Config();
//	/* 初始化系统滴答定时器 */

	/*蜂鸣器端口初始化 */
	Beep_GPIO_Config();
	//设置芯片读保护
//	Flash_EnableReadProtection();

	//	  /* 初始化通用定时器定时，20m+s产生一次中断 */
	TIMx_Configuration();

	/*CH376引脚配置初始化*/
	CH376_GPIO_Init();

	Touch_Init();
//	Delay_ms(100);

	Init_CH376();

	/* RTC配置：选择时钟源，设置RTC_CLK的分频系数 */
	RTC_CLK_Config();

	if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)

		/* 设置时间和日期 */
		RTC_TimeAndDate_Set();

	else
	{
	/* 检查是否电源复位 */
	if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
	{
//		 printf("\r\n 发生电源复位....\r\n");
	}
	/* 检查是否外部复位 */
	else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
	{
//		 printf("\r\n 发生外部复位....\r\n");
	}

	/* 使能 PWR 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
	PWR_BackupAccessCmd(ENABLE);
	/* 等待 RTC APB 寄存器同步 */
	RTC_WaitForSynchro();
	}

	USBD_Init(&USB_OTG_dev,USB_OTG_HS_CORE_ID,
           &USR_desc,&USBD_HID_cb,&USR_cb);
		/* 获取 SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	if (FlashID == sFLASH_ID) 
	{
//		Delay(500);
		Read_flag();
		Read_Flash_Init_Handle();
	}
//	watch = CRC16(test,9);
//	
//	watch = sizeof(TempHLimits);
	for(j=0;j<50000;j++)
	Delay(1000);
	for(j=0;j<16;j++)
	memset(filename[j],0,10);
	memset(newname,0,10);
	
uartflag = 1;
	while(1)
	{
//		LED1_ON;
//		ledstat = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
//		watch = GPIO_ReadInputDataBit(TOUCH_YPLUS_GPIO_PORT,TOUCH_YPLUS_GPIO_PIN);
/* 显示时间和日期 */	

		powerstat = GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11);
		/*if(powerstat == 0 && page_flag != poweron && page_flag != poweroff)
		{
			PowerOffHandle();
		}else*/
		{
		RTC_TimeAndDate_Show();
			/*按键扫描*/
			Key_Function();
//			if(TOUCH == op_on)
//			{
//				if(touchflag == 1)
//				{
////					Delay(1000);
//					TouchHandle(XCOOR,YCOOR);
//				}
//			}
			if(page_flag != poweron)
			{
				if(usavecount == 100)
				{
					udisk_scan();
					usavecount = 0;
				}
				else
					usavecount++;
			}
	//		Touch_Scan();
			DrawBattery(battery);
			
			if(uartflag == 1)
			{

					urecount = 0;

			}
			dispcnt++;//
			if(dispflag==0&&dispcnt>=300)
			{
				page_measure(xx);
        dispflag=1;
			}
//			xx++;
//				if(xx>999999)
//					xx=1;
//		strsum[0]='-';
////	strsum[0]='0'+xx/100000;
//	strsum[1]='0'+xx%100000/10000;
//	strsum[2]='0'+xx%10000/1000;
//	strsum[3]='.';			
//	strsum[4]='0'+xx%1000/100;
//	strsum[5]='0'+xx%100/10;
//	strsum[6]='0'+xx%10;
//	LCD_Lstring_Ex(24,265,72,120,strsum,0);
			
		}
	
	}
}




/*GERNERAL CODES*/
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

uint16_t CRC16(uint8_t *puchMsg, uint8_t Len)
{
	uint8_t t, m,n,p;
	uint8_t uchCRCHi=0xFF; /* 高CRC字节初始化*/ 
	uint8_t uchCRCLo =0xFF; /* 低CRC 字节初始化*/ 
	for(t=0;t<Len;t++)
	{	
		uchCRCLo=uchCRCLo^puchMsg[t];
		for(n=0;n<8;n++)
		{
			m=uchCRCLo&1;p=uchCRCHi&1;uchCRCHi>>=1;
			uchCRCLo>>=1;

			if(p)
			{
				uchCRCLo|=0x80;
			}
			if(m)	
			{
				uchCRCHi=uchCRCHi^0xa0;
				uchCRCLo=uchCRCLo^1;
			}
		}
	}
	return (uchCRCHi<<8|uchCRCLo);
}
//开机亮度
void InitBrt(void)
{
	if(BRTS == L0)
	{
		brightness = 10;
	}else if(BRTS == L1){
		brightness = 20;
	}else if(BRTS == L2){
		brightness = 40;
	}else if(BRTS == L3){
		brightness = 60;
	}else if(BRTS == L1){
		brightness = 80;
	}
	TIM_PWMOUTPUT_Config(brightness);
}


void Save_flag(void)
{
	SPI_FLASH_SectorErase(0);
	SPI_FLASH_BufferWrite((void*)savedata,SPI_FLASH_PageSize*0, sizeof(savedata));
	SPI_FLASH_BufferWrite((void*)TempHLimits,SPI_FLASH_PageSize*1, sizeof(TempHLimits));
	SPI_FLASH_BufferWrite((void*)TempLLimits,SPI_FLASH_PageSize*2, sizeof(TempLLimits));
	SPI_FLASH_BufferWrite((void*)Correction,SPI_FLASH_PageSize*4, sizeof(Correction));
	SPI_FLASH_BufferWrite((void*)corpara,SPI_FLASH_PageSize*5, sizeof(corpara));
//	Save_Sflag();
}

void Read_flag(void)
{
	SPI_FLASH_BufferRead((void *)savedata,SPI_FLASH_PageSize*0, sizeof(savedata));
	SPI_FLASH_BufferRead((void *)TempHLimits,SPI_FLASH_PageSize*1, sizeof(TempHLimits));
	SPI_FLASH_BufferRead((void *)TempLLimits,SPI_FLASH_PageSize*2, sizeof(TempLLimits));
	SPI_FLASH_BufferRead((void*)Correction,SPI_FLASH_PageSize*4, sizeof(Correction));
	SPI_FLASH_BufferRead((void*)corpara,SPI_FLASH_PageSize*5, sizeof(corpara));
	//	Read_history();
}




void UsbDataHandle(void)
{
	u8 i;
//	u8 j;
	uint16_t sendcrc;
	u8 creclen;
	u8 csendlen;

//	u16 voltage;//电压
//	u16 current;
//	u32 power;  //功率
//	u16 frequancy;
//	u16 PF;//功率因数
	
	if(usbbuf[0] == 0x01)
	{
		if(usbbuf[1] == 0x03)//读数据
		{
			free(crec);
			free(csend);
			if(usbbuf[2] == 0 && usbbuf[3] == 0)
			{
				readcrc = usbbuf[4] << 8|usbbuf[5];
				creclen = 4;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//初始化，每个元素都为零
				for(i = 0;i < 4;i++)
				{
					crec[i] = usbbuf[i];
				}
			}else{
				readcrc = usbbuf[6] << 8|usbbuf[7];
				creclen = 6;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//初始化，每个元素都为零
				for(i = 0;i < 6;i++)
				{
					crec[i] = usbbuf[i];
				}				
			}				
			crcwatch = CRC16(crec,creclen);
			if(CRC16(crec,creclen) == readcrc)//CRC校验
			{
				
				if(usbbuf[2] == 0 && usbbuf[3] == 0)//读实时数据
				{
					csendlen = 38;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = 0x00;
					usbsendbuf[5] = 0x10;
//					usbsendbuf[6] = usbbuf[5]*2;
					
//					if(usbsendbuf[5]<= 16)
//					{
						for(i = 0; i < usbsendbuf[5]; i++)
						{
							watch = (((u16)RecBuff[5+usbbuf[3]+i*2] << 8) + RecBuff[6+usbbuf[3]+i*2] - (int)(Correction[usbbuf[3] + i] * 10));
							if((((u16)RecBuff[5+usbbuf[3]+i*2] << 8) + RecBuff[6+usbbuf[3]+i*2] - (int)(Correction[usbbuf[3] + i] * 10)) == 0x4E1F)
							{
								usbsendbuf[6+i*2] = 0xFF;
								usbsendbuf[7+i*2] = 0xFF;
							}else{
								usbsendbuf[6+i*2] = (u8)((((u16)RecBuff[5+usbbuf[3]+i*2] << 8) + RecBuff[6+usbbuf[3]+i*2] - (int)(Correction[usbbuf[3] + i] * 10)) >> 8);
								usbsendbuf[7+i*2] = (u8)(((u16)RecBuff[5+usbbuf[3]+i*2] << 8) + RecBuff[6+usbbuf[3]+i*2] - (int)(Correction[usbbuf[3] + i] * 10));
							}
	//						usbsendbuf[7+i*2] = 0;
	//						usbsendbuf[8+i*2] = 0;
						}
						for(i = 0;i < csendlen; i++)
						{
							csend[i] = usbsendbuf[i];
						}
						sendcrc = CRC16(csend,csendlen);
						usbsendbuf[6+(usbsendbuf[5])*2] = (u8)(sendcrc >> 8);
						usbsendbuf[7+(usbsendbuf[5])*2] = (u8)(sendcrc);
//						for(i = 8 + (usbsendbuf[5])*2; i < 64 ; i++)
//						{
//							usbsendbuf[i] = 0;
//						}
						USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显						
//					}
//					else{
//						for(i = 0; i < usbbuf[5]; i++)
//						{
//							usbsendbuf[7+i*2] = 0xE0;
//							usbsendbuf[8+i*2] = 0xE0;
//	//						usbsendbuf[7+i*2] = 0;
//	//						usbsendbuf[8+i*2] = 0;
//						}
//						for(i = 0;i < csendlen; i++)
//						{
//							csend[i] = usbsendbuf[i];
//						}
//						sendcrc = CRC16(csend,csendlen);
//						usbsendbuf[7+(usbbuf[5])*2] = (u8)(sendcrc >> 8);
//						usbsendbuf[8+(usbbuf[5])*2] = (u8)(sendcrc);
//						USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
//					}
					
//					for(i = 0;i < 8;i++)
//					{
//						csendlen = 23;				
//						csend = (u8*)malloc(sizeof(u8) * csendlen);				
//						memset(csend, 0, csendlen);//初始化，每个元素都为零
//						usbsendbuf[0] = 0x01;
//						usbsendbuf[1] = 0x03;
//						usbsendbuf[2] = 0x00;
//						usbsendbuf[3] = 16*(i+1);
//						usbsendbuf[4] = 0x00;
//						usbsendbuf[5] = 16*(i+1)+16;
//						usbsendbuf[6] = 32;
//						for(j=0;j<16;j++)
//						{
//							usbsendbuf[7+j] = 0xee;
//							
//						}
//						for(j = 0;j < csendlen; j++)
//						{
//							csend[j] = usbsendbuf[j];
//						}
//						sendcrc = CRC16(csend,csendlen);
//						usbsendbuf[23] = (u8)(sendcrc >> 8);
//						usbsendbuf[24] = (u8)(sendcrc);
//						USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
//					}
				}else if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x00){//读取时间
					
					csendlen = 15;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);		
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度

//					usbreadtime = 1;
					
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x08;
					usbsendbuf[7] = usbreadtime[0];
					usbsendbuf[8] = usbreadtime[1];
					usbsendbuf[9] = usbreadtime[2];
					usbsendbuf[10] = usbreadtime[3];
					usbsendbuf[11] = usbreadtime[4];
					usbsendbuf[12] = usbreadtime[5];
					usbsendbuf[13] = usbreadtime[6];
					usbsendbuf[14] = 0;
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[15] = (u8)(sendcrc >> 8);
					usbsendbuf[16] = (u8)(sendcrc);
					for(i = 17; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					
					Delay(200);
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}else if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x20){//读取传感器类型
					
					csendlen = 9;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x02;
					usbsendbuf[7] = 0x00;
					usbsendbuf[8] = TCTYPE;
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[9] = (u8)(sendcrc >> 8);
					usbsendbuf[10] = (u8)(sendcrc);
					for(i = 11; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}else if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x10){//读取仪器状态
					
					csendlen = 9;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x02;
					usbsendbuf[7] = 0x00;
					usbsendbuf[8] = eqmtstatus;
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[9] = (u8)(sendcrc >> 8);
					usbsendbuf[10] = (u8)(sendcrc);
					for(i = 11; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}else if(usbbuf[2] == 0x01 && usbbuf[3] == 0x01){//读取上下限
					
					csendlen = 11;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x04;
					usbsendbuf[7] = (u8)((int)(TempHLimits[(usbbuf[3]-1)/2]) >> 8);
					usbsendbuf[8] = (u8)((int)(TempHLimits[(usbbuf[3]-1)/2]));
					usbsendbuf[9] = (u8)((int)(TempLLimits[(usbbuf[3]-1)/2]) >> 8);
					usbsendbuf[10] = (u8)((int)(TempLLimits[(usbbuf[3]-1)/2]));
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[11] = (u8)(sendcrc >> 8);
					usbsendbuf[12] = (u8)(sendcrc);
					for(i = 13; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}else if(usbbuf[2] == 0x02 && usbbuf[3] == 0x58){//读取单位
					free(csend);
					csendlen = 9;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x02;
					usbsendbuf[7] = 0x00;
					usbsendbuf[8] = UNIT;
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[9] = (u8)(sendcrc >> 8);
					usbsendbuf[10] = (u8)(sendcrc);
					for(i = 11; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}else if(usbbuf[2] == 0xEE && usbbuf[3] == 0xEE){//检测连接状态
					
					csendlen = 18;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x4A;
					usbsendbuf[7] = 0x4B;
					usbsendbuf[8] = 0x35;
					usbsendbuf[9] = 0x30;
					usbsendbuf[10] = 0x38;
					usbsendbuf[11] = 0x00;
					usbsendbuf[12] = 0x00;
					usbsendbuf[13] = CHNUM;
					usbsendbuf[14] = 0x00;
					usbsendbuf[15] = 0x00;
					usbsendbuf[16] = 0x00;
					usbsendbuf[17] = 0x00;

					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[18] = (u8)(sendcrc >> 8);
					usbsendbuf[19] = (u8)(sendcrc);
					for(i = 20; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
					
				}else if(usbbuf[2] == 0x80 && usbbuf[3] == 0x10){//读取电参数
					
					csendlen = 19;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					//发送数据CRC校验长度
					
					
//					usbsendbuf[0] = 0x00;
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x03;
					usbsendbuf[2] = usbbuf[2];
					usbsendbuf[3] = usbbuf[3];
					usbsendbuf[4] = usbbuf[4];
					usbsendbuf[5] = usbbuf[5];
					usbsendbuf[6] = 0x0C;
//					usbsendbuf[7] = (u8)(voltage >> 8);
//					usbsendbuf[8] = (u8)voltage;
//					usbsendbuf[9] = (u8)(current >> 8);
//					usbsendbuf[10] = (u8)current;
//					usbsendbuf[11] = (u8)(power >> 24);
//					usbsendbuf[12] = (u8)(power >> 16);
//					usbsendbuf[13] = (u8)(power >> 8);
//					usbsendbuf[14] = (u8)power;
//					usbsendbuf[15] = (u8)(frequancy >> 8);
//					usbsendbuf[16] = (u8)frequancy;
//					usbsendbuf[17] = (u8)(PF >> 8);
//					usbsendbuf[18] = (u8)PF;
					usbsendbuf[7] = 0xFF;
					usbsendbuf[8] = 0xFF;
					usbsendbuf[9] = 0xFF;
					usbsendbuf[10] = 0xFF;
					usbsendbuf[11] = 0xFF;
					usbsendbuf[12] = 0xFF;
					usbsendbuf[13] = 0xFF;
					usbsendbuf[14] = 0xFF;
					usbsendbuf[15] = 0xFF;
					usbsendbuf[16] = 0xFF;
					usbsendbuf[17] = 0xFF;
					usbsendbuf[18] = 0xFF;
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[19] = (u8)(sendcrc >> 8);
					usbsendbuf[20] = (u8)(sendcrc);
					
					for(i = 21; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}
			}
			
		}else if(usbbuf[1] == 0x10){
			free(crec);
			free(csend);
			if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x00)//设置时间
			{
				readcrc = usbbuf[15] << 8|usbbuf[16];
				creclen = 15;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//初始化，每个元素都为零
//				crec[0] = 0x01;
//				crec[1] = 0x10;
//				crec[2] = usbbuf[2];
//				crec[3] = usbbuf[3];
//				crec[4] = usbbuf[4];
//				crec[5] = usbbuf[5];
//				crec[6] = usbbuf[6];
//				crec[7] = usbbuf[7];
//				crec[8] = usbbuf[8];
//				crec[9] = usbbuf[9];
//				crec[10] = usbbuf[10];
//				crec[11] = usbbuf[11];
//				crec[12] = usbbuf[12];
//				crec[13] = usbbuf[13];
//				crec[14] = usbbuf[14];
				for(i = 0; i < creclen;i ++)
				{
					crec[i] = usbbuf[i];
				}
				
				crcwatch = CRC16(crec,creclen);
				if(CRC16(crec,creclen) == readcrc)
				{
					csendlen = 6;
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x10;
					usbsendbuf[2] = 0xC0;
					usbsendbuf[3] = 0x00;
					usbsendbuf[4] = 0x00;
					usbsendbuf[5] = 0x00;
					
					RTC_DateTypeDef RTC_DateStructure;
					RTC_TimeTypeDef RTC_TimeStructure;
					
					YEAR = usbbuf[8];
					MONTH = usbbuf[9];
					DATE = usbbuf[10];
					HOURS =usbbuf[11];
					MINUTES = usbbuf[12];
					SECONDS = usbbuf[13];										
					
					RTC_DateStructure.RTC_Date = DATE;
					RTC_DateStructure.RTC_Month = MONTH;         
					RTC_DateStructure.RTC_Year = YEAR;					
					RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
					RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
					
					RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
					RTC_TimeStructure.RTC_Hours = HOURS;        
					RTC_TimeStructure.RTC_Minutes = MINUTES;      
					RTC_TimeStructure.RTC_Seconds = SECONDS;      
					RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
					RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[6] = (u8)(sendcrc >> 8);
					usbsendbuf[7] = (u8)(sendcrc);
					
					for(i = 8; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}
			}else if(usbbuf[2] == 0x01){//设置上下限
				readcrc = usbbuf[11] << 8|usbbuf[12];
				creclen = 11;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//初始化，每个元素都为零
//				crec[0] = 0x01;
//				crec[1] = 0x10;
//				crec[2] = usbbuf[2];
//				crec[3] = usbbuf[3];
//				crec[4] = usbbuf[4];
//				crec[5] = usbbuf[5];
//				crec[6] = usbbuf[6];
//				crec[7] = usbbuf[7];
//				crec[8] = usbbuf[8];
//				crec[9] = usbbuf[9];
//				crec[10] = usbbuf[10];
				for(i = 0; i < creclen;i ++)
				{
					crec[i] = usbbuf[i];
				}
				crcwatch = CRC16(crec,creclen);
				if(CRC16(crec,creclen) == readcrc)
				{
					csendlen = 6;
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x10;
					usbsendbuf[2] = 0x01;
					usbsendbuf[3] = 0x01;
					usbsendbuf[4] = 0x00;
					usbsendbuf[5] = 0x02;
					
					TempHLimits[(usbbuf[3]-1)/2] = (float)(usbbuf[7] << 8 | usbbuf[8]);
					TempLLimits[(usbbuf[3]-1)/2] = (float)(usbbuf[9] << 8 | usbbuf[10]);
					Save_flag();
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[6] = (u8)(sendcrc >> 8);
					usbsendbuf[7] = (u8)(sendcrc);
					
					for(i = 8; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}
			}else if(usbbuf[2] == 0x02 && usbbuf[3] == 0x58){//设置单位
				readcrc = usbbuf[9] << 8|usbbuf[10];
				creclen = 9;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//初始化，每个元素都为零
//				crec[0] = 0x01;
//				crec[1] = 0x10;
//				crec[2] = usbbuf[2];
//				crec[3] = usbbuf[3];
//				crec[4] = usbbuf[4];
//				crec[5] = usbbuf[5];
//				crec[6] = usbbuf[6];
//				crec[7] = usbbuf[7];
//				crec[8] = usbbuf[8];
				for(i = 0; i < creclen;i ++)
				{
					crec[i] = usbbuf[i];
				}
				
				crcwatch = CRC16(crec,creclen);
				if(CRC16(crec,creclen) == readcrc)
				{
					csendlen = 6;
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//初始化，每个元素都为零
					usbsendbuf[0] = 0x01;
					usbsendbuf[1] = 0x10;
					usbsendbuf[2] = 0x02;
					usbsendbuf[3] = 0x58;
					usbsendbuf[4] = 0x00;
					usbsendbuf[5] = 0x01;
					
					UNIT = usbbuf[8];
					
					for(i = 0;i < csendlen; i++)
					{
						csend[i] = usbsendbuf[i];
					}
					sendcrc = CRC16(csend,csendlen);
					usbsendbuf[6] = (u8)(sendcrc >> 8);
					usbsendbuf[7] = (u8)(sendcrc);
					
					for(i = 8; i < 64 ; i++)
					{
						usbsendbuf[i] = 0;
					}
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//数据回显
				}
			}
		}
	}
}

//U盘检测
u8 udisk_scan(void)
{
	static u8 res;
	
//	u8 i;
//	char str[64];
	if(usbstatus != UNCONNECTED)
	{
		res = CH376DiskConnect( );
		if( res != USB_INT_SUCCESS )/* 检查U盘是否连接,等待U盘插入,对于SD卡,可以由单片机直接查询SD卡座的插拔状态引脚 */
		{  
			DrawUdisk1();
			usbstatus = UNCONNECTED;
			return NO_CONNECTION;
		}
	}
//	Delay(200);
	if(usbstatus != CONNECTED)
	{		
		res = CH376DiskMount( );
		if(res == USB_INT_SUCCESS && page_flag != touchcal)
		{
			DrawUdisk2();
			usbstatus = CONNECTED;
			return UDISK_READY;
		}

	}
	return UDISK_NOTREADY;
}



//BCD转换
int hex_to_bcd(int data)
{
//   int binaryInput = 0x202; 
   int bcdResult = 0;
   int shift = 0;

//   printf("Binary: 0x%x (dec: %d)\n", binaryInput , binaryInput );

   while (data > 0) {
      bcdResult |= (data % 10) << (shift++ << 2);
      data /= 10;
   }

//   printf("BCD: 0x%x (dec: %d)\n", bcdResult , bcdResult );
   return bcdResult;
}

//flash全部擦除
void Erase_all(void)
{
	static u16 serec = 2;
//	static u8 Check[4096];
	SPI_FLASH_SectorErase(serec*4096);
	SPI_FLASH_SectorErase(15880192+(serec-2)*4096);
	Delay(500);
//	SPI_FLASH_BufferRead((void *)Check,serec*4096, sizeof(Check));
	serec ++;

}

//读flash初始值处理
void Read_Flash_Init_Handle(void)
{
	u16 i;
	for(i=0;i<39;i++)
	{
		if(Correction[i] < -1800)
		{
			Correction[i] = 0;
		}
		if(TempHLimits[i] < -1800)
		{
			TempHLimits[i] = 0;
		}
		if(TempLLimits[i] < -1800)
		{
			TempLLimits[i] = 0;
		}
	}
	
	if(KEYLOCK > 1)
	{
		KEYLOCK = 0;
	}
	
}
//关机检测
u8 PowerOffDetect(void)
{
	return GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11);
}
//检测到关机后的处理
void PowerOffHandle(void)
{
	page_flag = poweroff;
	static u8 offflag;
	DrawPowOff();

	
}


/*********************************************END OF FILE**********************/

