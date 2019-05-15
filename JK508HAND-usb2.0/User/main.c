/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDCҺ����ʾ����(��ʾ�����С)
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
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
#include "./adc/adc.h"
#include <string.h>
#include <math.h>
#include <stdbool.h> 
/** @defgroup APP_HID_Private_Variables
  * @{
  */ 



void UARTRECHANDLE(void);
void Test_Process(void);
u8 DispBuf[DISP_MAX_LEN];//��ʾ����������
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
vu16 Range_value;
extern u8 uartflag;
//u8 p1,p2,p3,p4,p5,p6,p7,p8;
const u8 send_dot[]={2,2,4,3,2,4,3,2,4,3,2};
const u8 Send_uint[]={0,0,1,1,1,2,2,2,3,3,3};
Test_ValueTypedef Datacov(float value,u8 range)
{
    Test_ValueTypedef midvalue;
    midvalue.res=value;
    midvalue.dot=send_dot[range];
    midvalue.uint=Send_uint[range];
    return midvalue;
}


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
u8 Test_Uint[][3]=
{
    {"m"},
    {" "},
    {"k"},
};

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
       unsigned char  TC:4;    //�ȵ�ż����
       unsigned char  FT:1;    //��ѡ/�Ƚ�
       unsigned char  BP:1;    //Ѷ��
       unsigned char  UT:2;	   //��λ
   }BIT_FLAG;
}FLAG6;

union 
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

union 
{
   unsigned char FLAG_VAL8;
	
   struct 
   {
       unsigned char  BR:3;    //����
       unsigned char  SP:3;    //�ٶ�
       
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

//��ȡ��ID�洢λ��
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;


void Delay(__IO u32 nCount); 
void LCD_Test(void);
void Printf_Charater(void);
int i;
u16 count = 0;

float ch_temp[40];

/**
  * @brief  ������
  * @param  ��
  * @retval ��
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

		/*��ʼ��SDRAMģ��*/
	SDRAM_Init();
	 /*��ʼ��Һ����*/
    LCD_Init();

    LCD_LayerInit();
    LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_Clear(LCD_COLOR_BLACK);
	ADC_GPIO_CONFIG();
	SysTick_Init();
	Delay(500);
	
	/* LED �˿ڳ�ʼ�� */
//	LED_GPIO_Config();	 
	power_on();
	/* 16M����flash W25Q128��ʼ�� */
	SPI_FLASH_Init();
	
		/*���ڳ�ʼ��*/
//	Debug_USART_Config();
	 
	/* ��ȡ Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
	Touch_GPIO_Config();
	Delay(200);

	/*������ʼ��*/
	Key_GPIO_Config();
//	/* ��ʼ��ϵͳ�δ�ʱ�� */

	/*�������˿ڳ�ʼ�� */
	Beep_GPIO_Config();
	//����оƬ������
//	Flash_EnableReadProtection();

	//	  /* ��ʼ��ͨ�ö�ʱ����ʱ��20m+s����һ���ж� */
	TIMx_Configuration();

	/*CH376�������ó�ʼ��*/
	CH376_GPIO_Init();

	Touch_Init();
//	Delay_ms(100);

	Init_CH376();

	/* RTC���ã�ѡ��ʱ��Դ������RTC_CLK�ķ�Ƶϵ�� */
	RTC_CLK_Config();

	if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)

		/* ����ʱ������� */
		RTC_TimeAndDate_Set();

	else
	{
	/* ����Ƿ��Դ��λ */
	if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
	{
//		 printf("\r\n ������Դ��λ....\r\n");
	}
	/* ����Ƿ��ⲿ��λ */
	else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
	{
//		 printf("\r\n �����ⲿ��λ....\r\n");
	}

	/* ʹ�� PWR ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
	PWR_BackupAccessCmd(ENABLE);
	/* �ȴ� RTC APB �Ĵ���ͬ�� */
	RTC_WaitForSynchro();
	}
	
	INIT_ADS1216(0,0,0x01);//AD��ʼ��
	Range_out(Range);
//	USBD_Init(&USB_OTG_dev,USB_OTG_HS_CORE_ID,
//           &USR_desc,&USBD_HID_cb,&USR_cb);
//		/* ��ȡ SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	if (FlashID == sFLASH_ID) 
	{
//		Delay(500);
		Read_flag();
		Read_Flash_Init_Handle();
	}
//	watch = CRC16(test,9);
//	
	watch = sizeof(TempHLimits);
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
/* ��ʾʱ������� */	

		powerstat = GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11);
		/*if(powerstat == 0 && page_flag != poweron && page_flag != poweroff)
		{
			PowerOffHandle();
		}else*/
		{
		RTC_TimeAndDate_Show();
			/*����ɨ��*/
			Key_Function();
//			if(TOUCH == op_on)
//			{
//				if(touchflag == 1)
//				{
////					Delay(1000);
//					TouchHandle(XCOOR,YCOOR);
//				}
//			}
//			if(page_flag != poweron)
//			{
//				if(usavecount == 100)
//				{
//					udisk_scan();
//					usavecount = 0;
//				}
//				else
//					usavecount++;
//			}
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
			Test_Process();
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
//==========================================================
//�������ƣ�Hex_Format
//�������ܣ���ʽ������ֵ��DispBuf
//��ڲ�����������С��λ������ֵλ������ʾ��
//���ڲ�������
//�������ڣ�2014.12.15
//�޸����ڣ�2015.01.14 13:12
//��ע˵����dot=0-XXXXX XXXX.X XXX.XX XX.XXX X.XXXX-Dot=4
//ע��������5λ��
//==========================================================
void Hex_Format(u16 dat, u8 Dot, u8 len, bool dispzero)
{
	u8 i,j;

	if(len>5)len=5;//��󳤶�5
	if(len==0)len=1;
	if(Dot>4)Dot=4;//���С����4
	if(Dot>len)Dot=len;

	for(i=0;i<sizeof(DispBuf);i++)	//��ջ�����
		DispBuf[i]=' ';

	for(i=len;i>0;i--)	//����ת��
	{
		DispBuf[i-1] = dat%10 + '0';//תΪASCII
		dat/=10;
	}

	if(Dot)//��С����
	{
		j=len;
		i=Dot;//ȡС����λ��
		while(i--)
		{
			DispBuf[j]=DispBuf[j-1];
			j--;
		}
		DispBuf[j]='.';//����С����
	}

	if(dispzero==FALSE)//����ʾ��Ч��
	{
		for(i=0;i<(len-1);i++)
		{
			if((DispBuf[i]=='0')&&(DispBuf[i+1]!='.'))
				DispBuf[i]=' ';
			else	
				break;
		}
	}
	if(Dot)len++;
	for(i=len;i<sizeof(DispBuf);i++)//ĩβ��������'\0'
			DispBuf[i]=0;
	}

void Test_Process(void)
{
static s32 kx,Adx[8]={0};
s16 RX;
s32 k,kn;
	static u8 mn=0;
		//��ȡA/Dֵ
u8  Str[10]={0},m;
		k=	kn=AD_Convert_read();	//	Read_Ad();//��ȡADֵRead_Channel
 
	if(Ad_over)
	{
		Ad_over=0;
		if(!Savedata.adj[Range])
			Savedata.adj[Range]=10000;
		
		Savedata.adj[Range]=2215;
		Savedata.Zer0[Range]=00;
						 
		RX=Adx[mn]=10*(kn-Savedata.Zer0[Range])/Savedata.adj[Range];

		if(RX<0)
			RX=-RX;

		Range_adj(RX);	//Range_value���� �Ƚ�	
		if(mn==1)
		{//-<-<-
	/*		if(Adx[0]>Adx[1])
			{kn=Adx[0];
				Adx[0]=Adx[1];
				Adx[1]=kn;
			}
			if(Adx[1]>Adx[2])
			{kn=Adx[1];
				Adx[1]=Adx[2];
				Adx[2]=kn;
			}
			if(Adx[2]>Adx[3])
			{kn=Adx[2];
				Adx[2]=Adx[3];
				Adx[3]=kn;
			}
			if(Adx[3]>Adx[4])
			{kn=Adx[3];
				Adx[3]=Adx[4];
				Adx[4]=kn;
			}
			if(Adx[4]>Adx[5])
			{kn=Adx[4];
				Adx[4]=Adx[5];
				Adx[5]=kn;
			}
			if(Adx[5]>Adx[6])
			{kn=Adx[5];
				Adx[5]=Adx[6];
				Adx[6]=kn;
			}
			if(Adx[6]>Adx[7])
			{kn=Adx[6];
				Adx[6]=Adx[7];
				Adx[7]=kn;
			}*/

	//		kx=Adx[2]+Adx[3]+Adx[4]+Adx[5];
		kx=Adx[0]+Adx[1];//++Adx[6]+Adx[7]
		}
		mn++;
		if(mn>1)				mn=0;

		Res_count.r=RX;//kx>>1;
		if(Res_count.r<=0)      Range_value=0;
		else                    Range_value=Res_count.r;
	   
		if(Res_count.r<0)
		{
			Res_count.r=-Res_count.r;
			Test_Value.polar=1;
		}
		else
			Test_Value.polar=0;
			Test_Value=Datacov(Res_count.r,Range);

	//if(Jk516save.Compset.comp)//��ѡ
  //      if(Savedata.comp)//��ѡ
				//		F_Fail=Comp_choice();
//            Beep_Out(F_Fail);//��Ѷ����
//            Disp_Testvalue(Test_Value);
		Hex_Format(Test_Value.res , Test_Value.dot , 5, FALSE);
//				LCD_DispString_EN_CH( TESTVALUE_X, TESTVALUE_Y, DispBuf );
		LCD_Lstring_Ex(24,265,72,120,DispBuf,0);
		LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
		DISP_TEMP_L(340,581,Test_Uint[Test_Value.uint],0);
					

	}
	//	Uart_Process();//���ڴ���
   //     key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//�ȴ�����(100*10ms/10=100ms)
}


/*GERNERAL CODES*/
void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}

uint16_t CRC16(uint8_t *puchMsg, uint8_t Len)
{
	uint8_t t, m,n,p;
	uint8_t uchCRCHi=0xFF; /* ��CRC�ֽڳ�ʼ��*/ 
	uint8_t uchCRCLo =0xFF; /* ��CRC �ֽڳ�ʼ��*/ 
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
//��������
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

//	u16 voltage;//��ѹ
//	u16 current;
//	u32 power;  //����
//	u16 frequancy;
//	u16 PF;//��������
	
	if(usbbuf[0] == 0x01)
	{
		if(usbbuf[1] == 0x03)//������
		{
			free(crec);
			free(csend);
			if(usbbuf[2] == 0 && usbbuf[3] == 0)
			{
				readcrc = usbbuf[4] << 8|usbbuf[5];
				creclen = 4;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
				for(i = 0;i < 4;i++)
				{
					crec[i] = usbbuf[i];
				}
			}else{
				readcrc = usbbuf[6] << 8|usbbuf[7];
				creclen = 6;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
				for(i = 0;i < 6;i++)
				{
					crec[i] = usbbuf[i];
				}				
			}				
			crcwatch = CRC16(crec,creclen);
			if(CRC16(crec,creclen) == readcrc)//CRCУ��
			{
				
				if(usbbuf[2] == 0 && usbbuf[3] == 0)//��ʵʱ����
				{
					csendlen = 38;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
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
						USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���						
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
//						USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
//					}
					
//					for(i = 0;i < 8;i++)
//					{
//						csendlen = 23;				
//						csend = (u8*)malloc(sizeof(u8) * csendlen);				
//						memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
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
//						USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
//					}
				}else if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x00){//��ȡʱ��
					
					csendlen = 15;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);		
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��

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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}else if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x20){//��ȡ����������
					
					csendlen = 9;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
					
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
					
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}else if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x10){//��ȡ����״̬
					
					csendlen = 9;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
					
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}else if(usbbuf[2] == 0x01 && usbbuf[3] == 0x01){//��ȡ������
					
					csendlen = 11;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}else if(usbbuf[2] == 0x02 && usbbuf[3] == 0x58){//��ȡ��λ
					free(csend);
					csendlen = 9;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
					
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}else if(usbbuf[2] == 0xEE && usbbuf[3] == 0xEE){//�������״̬
					
					csendlen = 18;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
					
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
					
				}else if(usbbuf[2] == 0x80 && usbbuf[3] == 0x10){//��ȡ�����
					
					csendlen = 19;				
					csend = (u8*)malloc(sizeof(u8) * csendlen);				
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					//��������CRCУ�鳤��
					
					
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
					
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}
			}
			
		}else if(usbbuf[1] == 0x10){
			free(crec);
			free(csend);
			if(usbbuf[2] == 0xC0 && usbbuf[3] == 0x00)//����ʱ��
			{
				readcrc = usbbuf[15] << 8|usbbuf[16];
				creclen = 15;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
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
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
					
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}
			}else if(usbbuf[2] == 0x01){//����������
				readcrc = usbbuf[11] << 8|usbbuf[12];
				creclen = 11;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
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
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}
			}else if(usbbuf[2] == 0x02 && usbbuf[3] == 0x58){//���õ�λ
				readcrc = usbbuf[9] << 8|usbbuf[10];
				creclen = 9;
				crec = (u8 *)malloc(sizeof(u8) * creclen);
				memset(crec, 0, creclen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
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
					memset(csend, 0, csendlen);//��ʼ����ÿ��Ԫ�ض�Ϊ��
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
					USBD_HID_SendReport(&USB_OTG_dev,usbsendbuf,64);//���ݻ���
				}
			}
		}
	}
}

//U�̼��
u8 udisk_scan(void)
{
	static u8 res;
	
//	u8 i;
//	char str[64];
	if(usbstatus != UNCONNECTED)
	{
		res = CH376DiskConnect( );
		if( res != USB_INT_SUCCESS )/* ���U���Ƿ�����,�ȴ�U�̲���,����SD��,�����ɵ�Ƭ��ֱ�Ӳ�ѯSD�����Ĳ��״̬���� */
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



//BCDת��
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

//flashȫ������
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

//��flash��ʼֵ����
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
//�ػ����
u8 PowerOffDetect(void)
{
	return GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_11);
}
//��⵽�ػ���Ĵ���
void PowerOffHandle(void)
{
	page_flag = poweroff;
	static u8 offflag;
	DrawPowOff();

	
}


/*********************************************END OF FILE**********************/

