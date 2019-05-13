/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ������ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_basic_tim.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./ch376/ch376.h"
#include  "usbd_hid_core.h"
#include "./beep/bsp_beep.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"
#include "jk508.h"

void MODS_Poll(void);
void RecHandle(void);

extern u8 key_value;
extern u16 count;
extern u8 count_flag;
extern u8 uartflag;
u8 tempreq[8] = {0x01,0x03,0x00,0x00,0x00,0x10,0x44,0x06};
u8 reqcode;
u8 brightness;
u32 Tick_10ms=0;
u32 OldTick;
u8 udflag;
extern u8 ReCount;
extern u8 g_mods_timeout;
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
 /**
  * @brief  ������ʱ�� TIMx,x[6,7]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM7_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = BEEP_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// ����TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(BASIC_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 100-1;       
	
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(BASIC_TIM, ENABLE);	
}

static void TIM_7_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// ����TIMx_CLK,x[6,7] 
	RCC_APB1PeriphClockCmd(BEEP_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.TIM_Period = 10-1;       
	
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(BEEP_TIM, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(BEEP_TIM, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(BEEP_TIM,TIM_IT_Update,ENABLE);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(BEEP_TIM, ENABLE);	
}


/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (LTDC_BL_GPIO_CLK, ENABLE); 
  /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(LTDC_BL_GPIO_PORT,GENERAL_OCPWM_PINSOURCE,GENERAL_OCPWM_AF); 
  
	/* ��ʱ��ͨ���������� */															   
	GPIO_InitStructure.GPIO_Pin = LTDC_BL_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStructure);
}



/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
void TIM_PWMOUTPUT_Config(u8 duty)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// ����TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������8999����Ϊ9000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 100-1;       
	
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=90MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	/*PWMģʽ����*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = duty-1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);	 //ʹ��ͨ��3
  
	/*ʹ��ͨ��1����*/
	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(GENERAL_TIM, ENABLE);	
}

/**
  * @brief  ��ʼ��������ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{
	TIMx_NVIC_Configuration();	
    TIM_Mode_Config();
	
//	TIM_7_Config();
//	TIM7_NVIC_Configuration();
	
	TIMx_GPIO_Config();  
	TIM_PWMOUTPUT_Config(brightness);
}

void BEEP_TIM_IRQHandler(void)
{
	static u8 beepsw;
	if(TIM_GetITStatus( BEEP_TIM, TIM_IT_Update) != RESET )
	{
		if(beepsw == 0)
		{
			BEEP_ON;
		}else{
			BEEP_OFF;
		}
		beepsw = !beepsw;
		TIM_ClearITPendingBit(BEEP_TIM,TIM_IT_Update);
	}
}

void BASIC_TIM_IRQHandler (void)
{
	static u8 sendcount;
//	static u16 usavecount;
	static u32 dim_time;
	static u8 dimflag;
	
	u8 i;
	if(TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET )
	{
		Key_Scan();//����ɨ��
		Touch_Scan();//����ɨ��


		if(key_value == 0xFF && dimflag == 0)
		{			
			if(DIM == DOFF)
			{				
			}else if(DIM == D5){
				if(dim_time < 7500*4)
				{
					dim_time++;
				}else if(dim_time == 7500*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}else if(DIM == D10){
				if(dim_time < 15000*4)
				{
					dim_time++;
				}else if(dim_time == 15000*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}else if(DIM == D15){
				if(dim_time < 22500*4)
				{
					dim_time++;
				}else if(dim_time == 22500*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}else if(DIM == D30){
				if(dim_time < 45000*4)
				{
					dim_time++;
				}else if(dim_time == 45000*4)
				{
					TIM_PWMOUTPUT_Config(10);
					dimflag = 1;
				}
			}			
		}else if(key_value != 0xff){
			if(dim_time == (DIM * 7500*4))
			{
				TIM_PWMOUTPUT_Config(brightness);
				dimflag = 0;
			}
			dim_time = 0;
		}
		sendcount ++;
//		usavecount ++;
//		usave ++;
		TIM_ClearITPendingBit(BASIC_TIM,TIM_IT_Update);
	}
}

void MODS_Poll(void)
{
	static u32 testi;
	testi = ReCount;
	//�ж�ͨѶ�����Ƿ�ʱ
	if(OldTick!=Tick_10ms)
  	{  
	  OldTick=Tick_10ms;
	   if(g_mods_timeout>0)
	    g_mods_timeout--;
	  if(g_mods_timeout==0 && ReCount>0)   //��������ʱ��
      { 
		// goto err_ret;
	      }
      else if(g_mods_timeout==0 && ReCount==0) //û����ʱ��
         return;
      else //û��ʱ�ˣ�������
         return;
	}
	else   //û�е�10ms�����������
		return;
	//g_mods_timeout = 0;	 					/* ���־ */

	if (ReCount < 30)				/* ���յ�������С��4���ֽھ���Ϊ���� */
	{
		goto err_ret;
	}
	err_ret:
//	#if 0										/* 此部分为了串口打印结�?实际运用中可不要 */
//		g_tPrint.Rxlen = g_tModS.RxCount;
//		memcpy(g_tPrint.RxBuf, g_tModS.RxBuf, g_tModS.RxCount);

	//#endif
	
//	ReCount = 0;	
}



void SetTctype(u8 type)
{
	u8 tcbuf[27];
	u8 i;
	static u16 tccrc;
	u8 tcrc[25];
	
	tcbuf[0]=0X01;
	tcbuf[1]=0X10;
	tcbuf[2]=0X00;
	tcbuf[3]=0X00;
	tcbuf[4]=0X00;
	tcbuf[5]=0X09;
	tcbuf[6]=0X12;
	for(i=0;i<16;i++)
	{
		tcbuf[i+7]=type;
	}
	tcbuf[23] = UNIT;
	tcbuf[24] = CHNUM;
	for(i=0;i<25;i++)
	{
		tcrc[i] = tcbuf[i];
	}
	tccrc = CRC16(tcrc,25);
	tcbuf[26] = (u8)(tccrc >> 8);
	tcbuf[25] = (u8)(tccrc);
	for(i=0;i<27;i++)
	{
		Usart_SendByte(DEBUG_USART,tcbuf[i]);
	}
}
///**
//  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
//  * @param  ��
//  * @retval ��
//  */
//void TIMx_Configuration(void)
//{
//	TIMx_GPIO_Config();
//  
//  TIM_PWMOUTPUT_Config();
//}

/*********************************************END OF FILE**********************/
