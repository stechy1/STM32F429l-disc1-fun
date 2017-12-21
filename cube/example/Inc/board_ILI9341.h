/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "gfx.h"
//#include "ili9341_defines.h"
#include "../Drivers/BSP/Components/ili9341/ili9341.h"

#define SpixTimeout ((uint32_t)0x1000)

extern SPI_HandleTypeDef hspi5;

void ILI9341_SendCommand(uint8_t data);
void ILI9341_SendData(uint8_t data);


static GFXINLINE void init_board(GDisplay *g) {
	LCD_IO_Init();
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
	//HAL_GPIO_WritePin(ILI9341_RST_PORT, ILI9341_RST_PIN, !state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, !state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	//ILI9341_SendCommand((uint8_t) index);
	LCD_IO_WriteReg(index);

}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	//ILI9341_SendData((uint8_t) data);
	LCD_IO_WriteData(data);
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	return 0;
}

//void ILI9341_SendCommand(uint8_t data) {
//	ILI9341_WRX_RESET;
//	ILI9341_CS_RESET;
//	//HAL_SPI_Transmit(&hspi5, &data, sizeof(data), 1000);
//	HAL_SPI_Transmit(&hspi5, (uint8_t*) &data, 1, SpixTimeout);
//
//	ILI9341_CS_SET;
//}
//
//void ILI9341_SendData(uint8_t data) {
//	ILI9341_WRX_SET;
//	ILI9341_CS_RESET;
//	HAL_SPI_Transmit(&hspi5, (uint8_t*) &data, 1, SpixTimeout);
//	ILI9341_CS_SET;
//}

#endif /* _GDISP_LLD_BOARD_H */
