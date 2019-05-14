#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

//在滴答定时器中断服务函数中调用
void TimingDelay_Decrement(void);

// 初始化系统滴答定时器
void SysTick_Init(void);


//提供给应用程序调用
void delay_us(__IO u32 nTime);
#define delay_ms(x) delay_us(100*x)	 //单位ms

#endif /* __SYSTICK_H */
