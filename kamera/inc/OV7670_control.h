/*
*	==========================================================================
*   OV7670_control.h
*   OV7670 camera library.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Camera wiring:
*   3V3		-	3V		;		GND		-	GND
*   SIOC	-	PB8		;		SIOD	-	PB9
*   VSYNC -	PB7		;		HREF	-	PA4
*   PCLK	-	PA6		;		XCLK	-	PA8
*   D7		-	PE6		;		D6		-	PE5
*   D5		-	PD3(PB6);	D4		-	PE4
*   D3		-	PC9		;		D2		-	PC8
*   D1		-	PC7		;		D0		-	PC6
*   RESET	-	NC		;		PWDN	-	NC
*
*	==========================================================================
*/

// Includes
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_dcmi.h"
#include "lcd_ili9341.h"

#include "stm32f429i_discovery_sdram.h"

#include <stdbool.h>
#include <string.h>

extern uint32_t* gRAMBuffer;

//SDRAM defs
#define SDRAM_IMG          (uint32_t)(SDRAM_BANK_ADDR)
#define SDRAM_IMG0         (uint32_t)(SDRAM_IMG + 0x25800)
#define SDRAM_IMG1         (uint32_t)(SDRAM_IMG0 + 0x25800)
#define SDRAM_IMG2         (uint32_t)(SDRAM_IMG1 + 0x25800)
#define SDRAM_IMG3         (uint32_t)(SDRAM_IMG2 + 0x25800)

#define SDRAM_RGB888       (uint32_t)(SDRAM_IMG3 + 0x25800)
#define SDRAM_JPEG         (uint32_t)(SDRAM_RGB888 + 921600)  /* 3Byte* VGA */
#define SDRAM_ARR          (uint32_t)(SDRAM_JPEG + 133000)

#define IMG_LENGHT         (153600)   /* 153600 230040 0x38298 0x543e4 */
#define IMG_LENGHT_RGB888  (230400)

#define IS42S16400J_SIZE             0x400000

// SCCB write address
#define SCCB_REG_ADDR 			0x01

// OV7670 camera settings
#define OV7670_REG_NUM_VGA 		121 //VGA = 109 orig = 122 QVGA = 121
#define OV7670_REG_NUM_QVGA 	121 //VGA = 109 orig = 122 QVGA = 121
#define OV7670_WRITE_ADDR 	  0x42 //OV7670 = 0x42 OV5642=0x78
#define OV7670_READ_ADDR 	    0x43 //OV7670 = 0x43 OV5642=0x78

#define I2C_TIMEOUT           0x01FFFF

// QVGA Image settings
#define QVGA_IMG_ROWS   			320
#define QVGA_IMG_COLUMNS   	  240

// VGA Image settings
#define VGA_IMG_ROWS   				640
#define VGA_IMG_COLUMNS   		480

// Image settings
#define IMG_ROWS   					QVGA_IMG_ROWS
#define IMG_COLUMNS   			QVGA_IMG_COLUMNS

// Image buffer in CPU RAM
extern uint8_t CPURamBuffer[IMG_LENGHT];

//Initialize MCO1 (XCLK)
extern void MCO1Init(void);

//Initialize MCO1 (XCLK) alternative
extern uint8_t OV5642XCLKHwInit(void);

//Initialize SCCB
extern void SCCBInit(void);

//Initialize OV7670 camera
extern int OV7670Init(void);

//Initialize DCMI bus, DMA transfer
extern void DCMIDMAInit(void);

//Memory copy 32bit
extern void * memcpy32(void * dst, void const * src, size_t len);

//Erase 429i Discovery kit Memory
extern void eraseSDRAM(uint8_t data);

//UART4 init 429i Discovery kit
void UART4Init(uint32_t baudRate);

//USART1 init 429i Discovery kit
void USART1Init(uint32_t baudRate);

//memcpy32ExpRGB888
void memcpy32ExpRGB888(uint8_t *ldst, uint16_t *lsrc, size_t len);

