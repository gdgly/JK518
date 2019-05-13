#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

// ʱ��Դ�궨��
#define RTC_CLOCK_SOURCE_LSE      
//#define RTC_CLOCK_SOURCE_LSI

// �첽��Ƶ����
#define ASYNCHPREDIV         0X7F
// ͬ����Ƶ����
#define SYNCHPREDIV          0XFF
extern u8 date_time[6];



// ʱ��궨��
#define RTC_H12_AMorPM		 RTC_H12_AM  
#define HOURS                date_time[3]           // 0~23
#define MINUTES              date_time[4]           // 0~59
#define SECONDS              date_time[5]           // 0~59

// ���ں궨��
#define WEEKDAY              1         				// 1~7
#define DATE                 date_time[2]          // 1~31
#define MONTH                date_time[1]          // 1~12
#define YEAR                 date_time[0]         // 0~99

// ʱ���ʽ�궨��
#define RTC_Format_BINorBCD  RTC_Format_BIN

// ������Ĵ����궨��
#define RTC_BKP_DRX          RTC_BKP_DR0
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0X32F2
 
                                  
void RTC_CLK_Config(void);
void RTC_TimeAndDate_Set(void);
void RTC_TimeAndDate_Show(void);


#endif // __RTC_H__
