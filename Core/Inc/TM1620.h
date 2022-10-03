#ifndef _TM1620_H
#define _TM1620_H
#include "main.h"
#include "delay.h"

#define HAL_Delay(x) delay_ms(x)

#define MAX_TUBE_NUM					4			//数码管最大位数
#define MAX_TUBE_BRIGHTNESS		0x02	//固定亮度设置

int TM1620_Init(void);
void TM1620_Print(char* ch);
void TM1620_Display(uint16_t din);


#endif