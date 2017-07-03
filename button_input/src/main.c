/**
 ******************************************************************************
 * @file    main.c
 * @author  Petr Štechmüller
 * @version V1.0
 * @date    03.7.2017
 * @brief   Button input test
 ******************************************************************************
 */

#include "stm32f4xx.h"

int press = 0;

void delay(__IO uint32_t nCount) {
	while(nCount--) {}
}

void leds_init() {
	// Povolení hodin na kanálu G <A;K>
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	// Inicializační struktura pro konfiguraci LED diod
	GPIO_InitTypeDef GPIO_Led;
	GPIO_Led.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14; // Výběr konkrétních pinů
	GPIO_Led.GPIO_Mode = GPIO_Mode_OUT; // GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF, GPIO_Mode_AN
	GPIO_Led.GPIO_OType = GPIO_OType_PP;    // GPIO_OType_PP, GPIO_OType_OD
	GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL; // GPIO_PuPd_NOPULL, GPIO_PuPd_UP, GPIO_PuPd_DOWN
	GPIO_Led.GPIO_Speed = GPIO_Speed_50MHz; // GPIO_Speed_2MHz, GPIO_Speed_25MHz, GPIO_Speed_50MHz, GPIO_Speed_100MHz

	// Samotná inicializace LED na kanálu G <A;K>
	GPIO_Init(GPIOG, &GPIO_Led);
}

void btn_init() {
	// Povolení hodin na kanálu G <A;K>
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Inicializační struktura pro konfiguraci LED diod
	GPIO_InitTypeDef GPIO_Button;
	GPIO_Button.GPIO_Pin = GPIO_Pin_0;      // Výběr konkrétních pinů (tlačítko na desce)
	GPIO_Button.GPIO_Mode = GPIO_Mode_IN; // GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF, GPIO_Mode_AN
	GPIO_Button.GPIO_OType = GPIO_OType_PP;    // GPIO_OType_PP, GPIO_OType_OD
	GPIO_Button.GPIO_PuPd = GPIO_PuPd_DOWN; // GPIO_PuPd_NOPULL, GPIO_PuPd_UP, GPIO_PuPd_DOWN
	GPIO_Button.GPIO_Speed = GPIO_Speed_50MHz; // GPIO_Speed_2MHz, GPIO_Speed_25MHz, GPIO_Speed_50MHz, GPIO_Speed_100MHz

	// Samotná inicializace LED na kanálu G <A;K>
	GPIO_Init(GPIOA, &GPIO_Button);
}

int main(void) {
	// Toto volat při každém startu
	SystemInit();

	// Inicializace LED
	leds_init();

	// Inicializace tlačítek
	btn_init();

	for (;;) {
		// Přečtení vstupu z tlačítka
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) {
			if (press == 0) {
				press = 1;
			} else {
				press = 0;
			}
			delay(3000000);
		}

		if (press == 0) {
			GPIO_WriteBit(GPIOG, GPIO_Pin_13, Bit_SET);
			GPIO_WriteBit(GPIOG, GPIO_Pin_14, Bit_RESET);
		} else {
			GPIO_WriteBit(GPIOG, GPIO_Pin_13, Bit_RESET);
			GPIO_WriteBit(GPIOG, GPIO_Pin_14, Bit_SET);
		}
	}
}
