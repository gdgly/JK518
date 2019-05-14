#ifndef _adc_H
#define _adc_H


#define _4094_STBL GPIO_ResetBits(GPIOE,GPIO_Pin_6)
#define _4094_STBH GPIO_SetBits(GPIOE,GPIO_Pin_6)

#define _5618_CSL GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define _5618_CSH GPIO_SetBits(GPIOC,GPIO_Pin_3)

//#define _DS1302_CSL GPIO_ResetBits(GPIOB,GPIO_Pin_4)
//#define _DS1302_CSH GPIO_SetBits(GPIOB,GPIO_Pin_4)

//#define _CPLD_CSL GPIO_ResetBits(GPIOB,GPIO_Pin_8)
//#define _CPLD_CSH GPIO_SetBits(GPIOB,GPIO_Pin_8)

//hc595接口定义
//#define _SPI_595_CSL    GPIO_ResetBits(GPIOA,GPIO_Pin_11)
//#define _SPI_595_CSH    GPIO_SetBits(GPIOA,GPIO_Pin_11)

//#define _SPI_595_DSL    GPIO_ResetBits(GPIOA,GPIO_Pin_8)
//#define _SPI_595_DSH    GPIO_SetBits(GPIOA,GPIO_Pin_8)

//#define _SPI_595_SHCPL  GPIO_ResetBits(GPIOA,GPIO_Pin_9)
//#define _SPI_595_SHCPH  GPIO_SetBits(GPIOA,GPIO_Pin_9)

//#define _SPI_595_STCPL  GPIO_ResetBits(GPIOA,GPIO_Pin_10)
//#define _SPI_595_STCPH  GPIO_SetBits(GPIOA,GPIO_Pin_10)

//#define _7706_CSL GPIO_ResetBits(GPIOD,GPIO_Pin_14)
//#define _7706_CSH GPIO_SetBits(GPIOD,GPIO_Pin_14)

#define SPI_4094_SCLKL GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define SPI_4094_SCLKH GPIO_SetBits(GPIOC,GPIO_Pin_3)

#define SPI_4094_DATAL GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define SPI_4094_DATAH GPIO_SetBits(GPIOC,GPIO_Pin_2)

#define SPI_4094_STROL GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define SPI_4094_STROH GPIO_SetBits(GPIOC,GPIO_Pin_1)

#define ADS1216_CS_L GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define ADS1216_CS_H GPIO_SetBits(GPIOB,GPIO_Pin_10)
#define SPI_SCLKL GPIO_ResetBits(GPIOB,GPIO_Pin_11)
#define SPI_SCLKH GPIO_SetBits(GPIOB,GPIO_Pin_11)

//#define _SPI_DINL GPIO_ResetBits(GPIOB,GPIO_Pin_12)
//#define _SPI_DINH GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define SPI_DINL GPIO_ResetBits(GPIOC,GPIO_Pin_7)
#define SPI_DINH GPIO_SetBits(GPIOC,GPIO_Pin_7)

//#define _SPI_DOUT GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define SPI_DOUT GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)
//#define _SPI_DOUTH GPIO_SetBits(GPIOC,GPIO_Pin_10)

#define _FMQ_CSL GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define _FMQ_CSH GPIO_SetBits(GPIOB,GPIO_Pin_5)

//#define _TESTLED_CSL GPIO_ResetBits(GPIOE,GPIO_Pin_2)
//#define _TESTLED_CSH GPIO_SetBits(GPIOE,GPIO_Pin_2)

//电流量程上下限
#define RANGR_HIGH 21000
#define RANGR_LOW  1900
#define _4094NUM   1
#define CHANNEL_NUM             (15)
#define RANGE_MAX 			7//最大量程
#define DISP_MAX_LEN 8//显示缓冲区长度
extern vu8 Ad_over;
extern u8 va_4094[];
void INIT_ADS1216_Convert(u8 gpio,u8 pga,u8 ch);
void Range_out(u8 cha);
void ADC_GPIO_CONFIG(void);
s32 AD_Convert_read(void);
void Range_adj(u16 value);
typedef struct 
{
	vu16 value;	//数值
	vu8 dot;	//小数点 0 1 2 3    xxxx   xxx.x  xx.xx x.xxx
	vu8 unit;	//单位 0 1 2 3   MΩ    GΩ   TΩ  
}SystemRes_TypeDef;

typedef struct
{
	u8 Range;//量程  0 自动
	u8 language;
	u8 mode;
  u8 speed;
	u8 comp;
	u8 beep;
	u16 adj[RANGE_MAX];//校正值
	s16 Zero[CHANNEL_NUM];//清零值
	SystemRes_TypeDef Low;//下限
  SystemRes_TypeDef Hi;//上限
    s32 Zer0[CHANNEL_NUM];//清零值
}Savedata_Typedef;
extern Savedata_Typedef Savedata;
typedef struct
{
	float v;//温度
	float i;
	float r;//电阻值
	u32 rate;
}Res_countTypeDef;
extern Res_countTypeDef Res_count;
typedef struct
{
    int16_t res;
    u8 dot;
    u8 uint;
    u8 polar;
} Test_ValueTypedef;
extern Test_ValueTypedef Test_Value;
#endif
