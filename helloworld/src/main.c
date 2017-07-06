/**
 ******************************************************************************
 * @file    main.c
 * @author  Petr Štechmüller
 * @version V1.0
 * @date    03.7.2017
 * @brief   Hello world projekt
 ******************************************************************************
 */

#include "stm32f4xx.h"

void delay(__IO uint32_t nCount) {
	while(nCount--) {}
}

void init() {
	GPIO_InitTypeDef GPIO_Led;
	GPIO_Led.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Led.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Led.GPIO_OType = GPIO_OType_PP;
	GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Led.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOD, &GPIO_Led);

	GPIO_Led.GPIO_Pin = GPIO_Pin_13;
	GPIO_Led.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Led.GPIO_OType = GPIO_OType_PP;
	GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Led.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOG, &GPIO_Led);
}

int main(void) {

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	init();
	GPIO_WriteBit(GPIOD, GPIO_Pin_0, Bit_SET);

	for (;;) {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1);
		GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
		delay(16800000);
	}
}
