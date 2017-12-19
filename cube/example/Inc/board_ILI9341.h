/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "gfx.h"
#include "ili9341_defines.h"

extern SPI_HandleTypeDef hspi5;

void ILI9341_SendCommand(uint8_t data);
void ILI9341_SendData(uint8_t data);


static GFXINLINE void init_board(GDisplay *g) {
	/* Force reset */
	ILI9341_RST_RESET;
	gfxSleepMilliseconds(20000);
	ILI9341_RST_SET;

	/* Delay for RST response */
	gfxSleepMilliseconds(20000);

	/* Software reset */
	ILI9341_SendCommand(ILI9341_SOFT_RESET_REG);
	gfxSleepMilliseconds(50000);

	ILI9341_SendCommand(ILI9341_POWERCTLA_REG);
	ILI9341_SendData(0x39);
	ILI9341_SendData(0x2C);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x34);
	ILI9341_SendData(0x02);
	ILI9341_SendCommand(ILI9341_POWERCTLB_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0xC1);
	ILI9341_SendData(0x30);
	ILI9341_SendCommand(ILI9341_DIVTIMCTL_A_REG);
	ILI9341_SendData(0x85);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x78);
	ILI9341_SendCommand(ILI9341_DIVTIMCTL_B_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendCommand(ILI9341_POWONSEQCTL_REG);
	ILI9341_SendData(0x64);
	ILI9341_SendData(0x03);
	ILI9341_SendData(0x12);
	ILI9341_SendData(0x81);
	ILI9341_SendCommand(ILI9341_PUMPRATIOCTL_REG);
	ILI9341_SendData(0x20);
	ILI9341_SendCommand(ILI9341_POWERCTL1_REG);
	ILI9341_SendData(0x23);
	ILI9341_SendCommand(ILI9341_POWERCTL2_REG);
	ILI9341_SendData(0x10);
	ILI9341_SendCommand(ILI9341_VCOMCTL1_REG);
	ILI9341_SendData(0x3E);
	ILI9341_SendData(0x28);
	ILI9341_SendCommand(ILI9341_VCOMCTL2_REG);
	ILI9341_SendData(0x86);
	ILI9341_SendCommand(ILI9341_MEMACCESS_REG);
	ILI9341_SendData(0x48);
	ILI9341_SendCommand(ILI9341_PIXFORMATSET_REG);
	ILI9341_SendData(0x55);
	ILI9341_SendCommand(ILI9341_FRAMECTL_NOR_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x18);
	ILI9341_SendCommand(ILI9341_FUNCTONCTL_REG);
	ILI9341_SendData(0x08);
	ILI9341_SendData(0x82);
	ILI9341_SendData(0x27);
	ILI9341_SendCommand(ILI9341_ENABLE_3G_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendCommand(ILI9341_COLADDRSET_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0xEF);
	ILI9341_SendCommand(ILI9341_PAGEADDRSET_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x01);
	ILI9341_SendData(0x3F);
	ILI9341_SendCommand(ILI9341_GAMMASET_REG);
	ILI9341_SendData(0x01);
	ILI9341_SendCommand(ILI9341_POSGAMMACORRECTION_REG);
	ILI9341_SendData(0x0F);
	ILI9341_SendData(0x31);
	ILI9341_SendData(0x2B);
	ILI9341_SendData(0x0C);
	ILI9341_SendData(0x0E);
	ILI9341_SendData(0x08);
	ILI9341_SendData(0x4E);
	ILI9341_SendData(0xF1);
	ILI9341_SendData(0x37);
	ILI9341_SendData(0x07);
	ILI9341_SendData(0x10);
	ILI9341_SendData(0x03);
	ILI9341_SendData(0x0E);
	ILI9341_SendData(0x09);
	ILI9341_SendData(0x00);
	ILI9341_SendCommand(ILI9341_NEGGAMMACORRECTION_REG);
	ILI9341_SendData(0x00);
	ILI9341_SendData(0x0E);
	ILI9341_SendData(0x14);
	ILI9341_SendData(0x03);
	ILI9341_SendData(0x11);
	ILI9341_SendData(0x07);
	ILI9341_SendData(0x31);
	ILI9341_SendData(0xC1);
	ILI9341_SendData(0x48);
	ILI9341_SendData(0x08);
	ILI9341_SendData(0x0F);
	ILI9341_SendData(0x0C);
	ILI9341_SendData(0x31);
	ILI9341_SendData(0x36);
	ILI9341_SendData(0x0F);
	ILI9341_SendCommand(ILI9341_SLEEP_OUT_REG);

	gfxSleepMilliseconds(1000000);

	ILI9341_SendCommand(ILI9341_DISPLAYON_REG);
	ILI9341_SendCommand(ILI9341_MEMORYWRITE_REG);

}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
	HAL_GPIO_WritePin(ILI9341_RST_PORT, ILI9341_RST_PIN, !state ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
	ILI9341_SendCommand(index >> 8);
	ILI9341_SendCommand((uint8_t) index);
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	ILI9341_SendCommand(data >> 8);
	ILI9341_SendCommand((uint8_t) data);
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

void ILI9341_SendCommand(uint8_t data) {
	ILI9341_WRX_RESET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi5, &data, sizeof(data), 1000);

	ILI9341_CS_SET;
}

void ILI9341_SendData(uint8_t data) {
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;
	HAL_SPI_Transmit(&hspi5, &data, sizeof(data), 1000);
	ILI9341_CS_SET;
}

#endif /* _GDISP_LLD_BOARD_H */
