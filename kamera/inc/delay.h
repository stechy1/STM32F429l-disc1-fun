/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H
#define __DELAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

void mdelay(volatile uint32_t ms);

void delay_us(volatile uint32_t time);
void delay_ms(volatile uint32_t time);

#endif // __DELAY_H

