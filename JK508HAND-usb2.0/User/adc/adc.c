#include "stm32f4xx.h"
#include "adc.h"
#include "jk508.h"
#include "./systick/bsp_SysTick.h"
u8 buff_4094[_4094NUM];
//==========================================================
//#define POWERON_DISP_TIME (0)	//开机显示界面延时20*100mS=2s
u8 U15_4094,U16_4094;
//u8 _4051value[]={0x48,0x48,0x48,0x88,0x09,0x01,0x02,0x04,0x30};
///////////////10m 100m  1    10   100  1K   10K  100K
u8 va_4094[]={0x48,0x48,0x48,0x88,0x09,0x01,0x02,0x04};
//u8 va_4094[]={0x40,0x40,0x40,0x80,0x01,0x09,0x0a,0x0c};
//u8 va_4094[]={0x01,0x02,0x4,0x8,0x10,0x20,0x40,0x80};
u8 ctr_4052[]={2,2,1,1,1,0,0,0};
u8 pga_1216[]={3,3,1,1,1,0,0,0};
Savedata_Typedef Savedata;
Res_countTypeDef Res_count;
Test_ValueTypedef Test_Value;
vu8 Ad_over;
s32 Ad_value;//低阻AD采样值
vu8 Range;//当前量程


void ADC_GPIO_CONFIG(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);
	//4094控制端口

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_1);

    //ADS1216控制端口
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;|GPIO_Pin_12
	GPIO_Init(GPIOB,&GPIO_InitStructure);

     //ADS1216控制端口输入
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
 //   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;//13;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOD,&GPIO_InitStructure);

	///////////////////////////////////nn
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	
	//INIT_AD_Convert();//AD初始化
}



/* ************************************ */
/*函数名：SpisendByte(INT8U dat)*/
/*输入：发送的数据*/
/*输出：无*/
/*功能描述：SPI发送一个字节*/
/* ************************************ */
void SpiTxByte(u8 dat)
{
	u8 i;
    SPI_SCLKH;
    delay_us(1);
	for(i=0; i<8; i++)
    {
        SPI_SCLKL;
        if(dat & 0x80)
            SPI_DINH;
		else
			 SPI_DINL;
		dat <<= 1;
        delay_us(1);
		SPI_SCLKH;
        delay_us(1);
	}
}
/* ************************************ */
/*函数名：SpisendByte(INT8U dat)*/
/*输入：发送的数据*/
/*输出：无*/
/*功能描述：SPI接收一个字节*/
/* ************************************ */
u8 SpiRxByte(u8 rdry)
{	u8 i,temp;
//	temp = 0;
	if(rdry)
		SPI_DINH;
	else
		SPI_DINL;
    //delay_us(1);
	for(i=0; i<8; i++)
	{
        SPI_SCLKL;
        delay_us(1);
		SPI_SCLKH;
        temp <<= 1;
		if(SPI_DOUT)  /* CPU_IN */
			temp++;
        delay_us(1);
	}
	return temp;
}
//*command+data*/
void Wr_CmDat(u8 cmd,u8 dat)
{
	SpiTxByte(cmd);
	SpiTxByte(dat);
	}
/* ********************************************************************** */
/* 函数名void AD(void) */
/* 输入:无 */
/* 输出:无 */
/* 功能描述:AD转换结果送 u8 AD0_VALUE[]*/
/* ********************************************************************** */
void INIT_ADS1216(u8 gpio,u8 pga,u8 ain_a)
{//_Convert
		ADS1216_CS_L;
    delay_us(3);
	Wr_CmDat(0x50,01);
		SpiTxByte(0x1e);//SpiTxByte(0x0c);//内部基准 2.5V
	SpiTxByte(ain_a);//通道0
//	SpiTxByte(pga);//pga0x05
	ADS1216_CS_H;
	delay_us(13);
	
	ADS1216_CS_L;
    delay_us(3);
	Wr_CmDat(0x53,00);
	SpiTxByte(0x00);
	ADS1216_CS_H;
	delay_us(13);
	
	ADS1216_CS_L;
    delay_us(3);
	Wr_CmDat(0x57,02);
	SpiTxByte(0xfc);
	SpiTxByte(0x70);//采样速度c0
	SpiTxByte(0x37);//采样速度
	ADS1216_CS_H;
	//delay_us(13);
	
//	ADS1216_CS_L;
//    delay_us(3);
//		Wr_CmDat(0x19,00);
//		SpiTxByte(0x00);
//		ADS1216_CS_H;
//	delay_us(13);

      delay_ms(100);

}

void Set_4052(u8 port,u8 gain) // 0,1 2
{
		ADS1216_CS_L;
	delay_us(8);
    Wr_CmDat(0x52,00);
    delay_us(2);
    SpiTxByte(gain);
    delay_us(2);
    ADS1216_CS_H;
    delay_us(8);
	ADS1216_CS_L;
    delay_us(8);

    Wr_CmDat(0x56,00);
    delay_us(2);
    SpiTxByte(port);
    delay_us(2);
    ADS1216_CS_H;
    delay_ms(20);
}
/* ********************************************************************** */
/* 函数名void AD(void) */
/* 输入:无 */
/* 输出:无 */
/* 功能描述:AD转换结果送 u8 AD0_VALUE[]*/
/* ********************************************************************** */
//================
s32 AD_Convert_read(void)
{
    u8  temp;
	s32 Ad_adv=0;
    ADS1216_CS_L;
    delay_us(1);
    Wr_CmDat(0x19,00);//读M/DEC1,取/DRDY
    delay_us(1);
    temp=SpiRxByte(0);
	ADS1216_CS_H;
    temp=temp&0x80;
  if(!temp)
	{ADS1216_CS_L;
        SpiTxByte(0x01);
		ADS1216_CS_L;
		ADS1216_CS_L;
        Ad_adv=SpiRxByte(1);
        Ad_adv<<=8;
        Ad_adv|=SpiRxByte(1);
        Ad_adv<<=8;
        Ad_adv|=SpiRxByte(1);
        Ad_over=1;
	}
    ADS1216_CS_H;
	if(Ad_adv>0xff0000)
		Ad_adv|=0xff000000;
	return Ad_adv;
}

void Out_4094(u8 *buff) //写CD4094子函数;
{
	u8 i;       //定义子函数;
SPI_4094_STROL;
delay_us(30);
		for(i=0;i<8;i++)     //写八位数据进CD4094;
		{SPI_4094_SCLKL;       //形成上升延脉冲;
			delay_us(25);
			if(*(buff)&0x80)     //判断并发串行数据;
				SPI_4094_DATAH;      //发高电平;
			
			else
				SPI_4094_DATAL;      //发低电平;
			
			*(buff)<<=1;      //数据移位,送下一位数据;
			delay_us(25);
			SPI_4094_SCLKH;
			delay_us(25);
		}
		SPI_4094_DATAH;      //发低电平;
		SPI_4094_SCLKL;
SPI_4094_STROH;
}

void Range_out(u8 cha)
{
	u8 out52,gaina;

	buff_4094[0]=va_4094[cha];
  Out_4094(buff_4094);
	out52=ctr_4052[cha];
	gaina=pga_1216[cha];
	Set_4052(out52,gaina);
	delay_ms(2);
}
//==========================================================
//函数名称：Range_Changecomp
//函数功能：电阻测试换挡
//入口参数：无
//出口参数：无
//创建日期：2015.10.26 
//修改日期：2015.10.29 13:35
//备注说明：无
//==========================================================
//void Range_Changecomp(void)
void Range_adj(u16 value)
{
u8 static RNG1=100;
	//量程自动换挡处?
	if(value>RANGR_HIGH&&(Range<7))//高于量程上限Ad_value
	{
		Range++;
		RNG1=15;//量程切换标志
	}
	else if(value<RANGR_LOW&&(Range>0))//低于量程下限
	{RNG1=15;
		Range--;
	}

	if(RNG1!=Range)
		{
			Range_out(Range);//量程控制
		RNG1=Range;
		}
}