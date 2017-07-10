
/*
*	==========================================================================
*   OV7670_control.c
*   Description:
*   OV7670 camera file.
*   Optmz for 32F429IDISCO board.
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
*   3V3		-	3V		 ;		GND		-	GND
*   SIOC	-	PB8		 ;		SIOD	-	PB9
*   VSYNC -	PB7	   ;		HREF	-	PA4
*   PCLK	-	PA6		 ;		XCLK	-	PA8
*   D7		-	PE6		 ;		D6		-	PE5
*   D5		-	PD3(PB6);		D4		-	PE4
*   D3		-	PC9		 ;		D2		-	PC8
*   D1		-	PC7		 ;		D0		-	PC6
*   RESET	-	NC		 ;		PWDN	-	NC
*
*	==========================================================================
*/

#include "OV7670_control.h"
#include <stdio.h>
#include "errno.h"

// Image buffer
//uint16_t CPURamBuffer[IMG_ROWS*IMG_COLUMNS] = {0};

#define DCMI_TIMEOUT_MAX 10000

// Number of register to change value when initialize Camera Module
#define RegNum		134
#define CHANGE_REG	167
const uint8_t Camera_REG[CHANGE_REG][2] =
{
  { 0x3a, 0x04 },
  { 0x40, 0xd0 },
  { 0x12, 0x14 },
  { 0x32, 0x80 },
  { 0x17, 0x16 },
  { 0x18, 0x04 },
  { 0x19, 0x02 },
  { 0x1a, 0x7b },
  { 0x03, 0x06 },
  { 0x0c, 0x00 }, //0x00
  { 0x3e, 0x00 },
  { 0x70, 0x00 },
  { 0x71, 0x00 },
  { 0x72, 0x11 },
  { 0x73, 0x00 },
  { 0xa2, 0x02 },
  { 0x11, 0x40 },
  { 0x7a, 0x20 },
  { 0x7b, 0x1c },
  { 0x7c, 0x28 },
  { 0x7d, 0x3c },
  { 0x7e, 0x55 },
  { 0x7f, 0x68 },
  { 0x80, 0x76 },
  { 0x81, 0x80 },
  { 0x82, 0x88 },
  { 0x83, 0x8f },
  { 0x84, 0x96 },
  { 0x85, 0xa3 },
  { 0x86, 0xaf },
  { 0x87, 0xc4 },
  { 0x88, 0xd7 },
  { 0x89, 0xe8 },
  { 0x13, 0xe0 },
  { 0x00, 0x00 },
  { 0x10, 0x00 },
  { 0x0d, 0x00 },
  { 0x14, 0x20 },
  { 0xa5, 0x05 },
  { 0xab, 0x07 },
  { 0x24, 0x75 },
  { 0x25, 0x63 },
  { 0x26, 0xA5 },
  { 0x9f, 0x78 },
  { 0xa0, 0x68 },
  { 0xa1, 0x03 },
  { 0xa6, 0xdf },
  { 0xa7, 0xdf },
  { 0xa8, 0xf0 },
  { 0xa9, 0x90 },
  { 0xaa, 0x94 },
  { 0x13, 0xe5 },
  { 0x0e, 0x61 },
  { 0x0f, 0x4b },
  { 0x16, 0x02 },
  { 0x1e, 0x07 },//0x07,
  { 0x21, 0x02 },
  { 0x22, 0x91 },
  { 0x29, 0x07 },
  { 0x33, 0x0b },
  { 0x35, 0x0b },
  { 0x37, 0x1d },
  { 0x38, 0x71 },
  { 0x39, 0x2a },
  { 0x3c, 0x78 },
  { 0x4d, 0x40 },
  { 0x4e, 0x20 },
  { 0x69, 0x0c },
  { 0x6b, 0x60 },
  { 0x74, 0x19 },
  { 0x8d, 0x4f },
  { 0x8e, 0x00 },
  { 0x8f, 0x00 },
  { 0x90, 0x00 },
  { 0x91, 0x00 },
  { 0x92, 0x00 },
  { 0x96, 0x00 },
  { 0x9a, 0x80 },
  { 0xb0, 0x84 },
  { 0xb1, 0x0c },
  { 0xb2, 0x0e },
  { 0xb3, 0x82 },
  { 0xb8, 0x0a },
  { 0x43, 0x14 },
  { 0x44, 0xf0 },
  { 0x45, 0x34 },
  { 0x46, 0x58 },
  { 0x47, 0x28 },
  { 0x48, 0x3a },
  { 0x59, 0x88 },
  { 0x5a, 0x88 },
  { 0x5b, 0x44 },
  { 0x5c, 0x67 },
  { 0x5d, 0x49 },
  { 0x5e, 0x0e },
  { 0x64, 0x04 },
  { 0x65, 0x20 },
  { 0x66, 0x05 },
  { 0x94, 0x04 },
  { 0x95, 0x08 },
  { 0x6c, 0x0a },
  { 0x6d, 0x55 },
  { 0x6e, 0x11 },
  { 0x6f, 0x9f },
  { 0x6a, 0x40 },
  { 0x01, 0x40 },	//0x40
  { 0x02, 0x40 },
  { 0x13, 0xe7 },
  { 0x15, 0x00 },
  { 0x4f, 0x80 },
  { 0x50, 0x80 },
  { 0x51, 0x00 },
  { 0x52, 0x22 },
  { 0x53, 0x5e },
  { 0x54, 0x80 },
  { 0x58, 0x9e },
  { 0x41, 0x08 },
  { 0x3f, 0x00 },
  { 0x75, 0x05 },
  { 0x76, 0xe1 },
  { 0x4c, 0x00 },
  { 0x77, 0x01 },
  { 0x3d, 0xc2 },
  { 0x4b, 0x09 },
  { 0xc9, 0x60 },
  { 0x41, 0x38 },
  { 0x56, 0x40 },
  { 0x34, 0x11 },
  { 0x3b, 0x02 },
  { 0xa4, 0x89 },
  { 0x96, 0x00 },
  { 0x97, 0x30 },
  { 0x98, 0x20 },
  { 0x99, 0x30 },
  { 0x9a, 0x8  }, //14.11.2016 pøeklep, spravna hodnota _???
  { 0x9b, 0x29 },
  { 0x9c, 0x03 },
  { 0x9d, 0x4c },
  { 0x9e, 0x3f },
  { 0x78, 0x04 },
  { 0x79, 0x01 },
  { 0xc8, 0xf0 },
  { 0x79, 0x0f },
  { 0xc8, 0x00 },
  { 0x79, 0x10 },
  { 0xc8, 0x7e },
  { 0x79, 0x0a },
  { 0xc8, 0x80 },
  { 0x79, 0x0b },
  { 0xc8, 0x01 },
  { 0x79, 0x0c },
  { 0xc8, 0x0f },
  { 0x79, 0x0d },
  { 0xc8, 0x20 },
  { 0x79, 0x09 },
  { 0xc8, 0x80 },
  { 0x79, 0x02 },
  { 0xc8, 0xc0 },
  { 0x79, 0x03 },
  { 0xc8, 0x40 },
  { 0x79, 0x05 },
  { 0xc8, 0x30 },
  { 0x79, 0x26 },
  { 0x09, 0x01 },	 //0x03
  { 0x55, 0x00 },
  { 0x56, 0x40 },
  { 0x3b, 0x42 },
};

const uint8_t OV7670_regVGA[OV7670_REG_NUM_VGA][2] =
{
  //{0x12, 0x80},		//Reset registers
  // Image format
  { 0x12, 0x04 },		//0x14=QVGA size,
  { 0x40, 0xd0 },		//RGB565
  { 0xb0, 0x84 },		//Color mode

  // Hardware window

  { 0x11, 0x0F },		//PCLK settings, 15fps QVGA=0x01 VGA=0x0F
  { 0x32, 0x80 },		//HREF
  { 0x17, 0x17 },		//HSTART
  { 0x18, 0x05 },		//HSTOP
  { 0x03, 0x0a },		//VREF
  { 0x19, 0x02 },		//VSTART
  { 0x1a, 0x7a },		//VSTOP

  // Scalling numbers
  { 0x70, 0x3a },		//X_SCALING
  { 0x71, 0x35 },		//Y_SCALING
  { 0x72, 0x11 },		//DCW_SCALING
  { 0x73, 0xf0 },		//PCLK_DIV_SCALING
  { 0xa2, 0x02 },		//PCLK_DELAY_SCALING

  // Matrix coefficients
  { 0x4f, 0x80 },
  { 0x50, 0x80 },
  { 0x51, 0x00 },
  { 0x52, 0x22 },
  { 0x53, 0x5e },
  { 0x54, 0x80 },
  { 0x58, 0x9e },

  // Gamma curve values
  { 0x7a, 0x20 },
  { 0x7b, 0x10 },
  { 0x7c, 0x1e },
  { 0x7d, 0x35 },
  { 0x7e, 0x5a },
  { 0x7f, 0x69 },
  { 0x80, 0x76 },
  { 0x81, 0x80 },
  { 0x82, 0x88 },
  { 0x83, 0x8f },
  { 0x84, 0x96 },
  { 0x85, 0xa3 },
  { 0x86, 0xaf },
  { 0x87, 0xc4 },
  { 0x88, 0xd7 },
  { 0x89, 0xe8 },

  // AGC and AEC parameters
  { 0xa5, 0x05 },
  { 0xab, 0x07 },
  { 0x24, 0x95 },
  { 0x25, 0x33 },
  { 0x26, 0xe3 },
  { 0x9f, 0x78 },
  { 0xa0, 0x68 },
  { 0xa1, 0x03 },
  { 0xa6, 0xd8 },
  { 0xa7, 0xd8 },
  { 0xa8, 0xf0 },
  { 0xa9, 0x90 },
  { 0xaa, 0x94 },
  { 0x10, 0x00 },

  // AWB parameters
  { 0x43, 0x0a },
  { 0x44, 0xf0 },
  { 0x45, 0x34 },
  { 0x46, 0x58 },
  { 0x47, 0x28 },
  { 0x48, 0x3a },
  { 0x59, 0x88 },
  { 0x5a, 0x88 },
  { 0x5b, 0x44 },
  { 0x5c, 0x67 },
  { 0x5d, 0x49 },
  { 0x5e, 0x0e },
  { 0x6c, 0x0a },
  { 0x6d, 0x55 },
  { 0x6e, 0x11 },
  { 0x6f, 0x9f },
  { 0x6a, 0x40 },
  { 0x01, 0x40 },
  { 0x02, 0x60 },
  { 0x13, 0xe7 },

  // Additional parameters
  { 0x34, 0x11 },
  { 0x3f, 0x00 },
  { 0x75, 0x05 },
  { 0x76, 0xe1 },
  { 0x4c, 0x00 },
  { 0x77, 0x01 },
  { 0xb8, 0x0a },
  { 0x41, 0x18 },
  { 0x3b, 0x12 },
  { 0xa4, 0x88 },
  { 0x96, 0x00 },
  { 0x97, 0x30 },
  { 0x98, 0x20 },
  { 0x99, 0x30 },
  { 0x9a, 0x84 },
  { 0x9b, 0x29 },
  { 0x9c, 0x03 },
  { 0x9d, 0x4c },
  { 0x9e, 0x3f },
  { 0x78, 0x04 },
  { 0x0e, 0x61 },
  { 0x0f, 0x4b },
  { 0x16, 0x02 },
  { 0x1e, 0x00 },
  { 0x21, 0x02 },
  { 0x22, 0x91 },
  { 0x29, 0x07 },
  { 0x33, 0x0b },
  { 0x35, 0x0b },
  { 0x37, 0x1d },
  { 0x38, 0x71 },
  { 0x39, 0x2a },
  { 0x3c, 0x78 },
  { 0x4d, 0x40 },
  { 0x4e, 0x20 },
  { 0x69, 0x00 },
  { 0x6b, 0x3a },
  { 0x74, 0x10 },
  { 0x8d, 0x4f },
  { 0x8e, 0x00 },
  { 0x8f, 0x00 },
  { 0x90, 0x00 },
  { 0x91, 0x00 },
  { 0x96, 0x00 },
  { 0x9a, 0x00 },
  { 0xb1, 0x0c },
  { 0xb2, 0x0e },
  { 0xb3, 0x82 },
  { 0x4b, 0x01 },
};

const uint8_t OV7670_regQVGA[OV7670_REG_NUM_QVGA][2] =
{
  //{0x12, 0x80},		//Reset registers
  // Image format
  {0x12, 0x14},		//QVGA size, RGB mode
  {0x40, 0xd0},		//RGB565
  {0xb0, 0x84},		//Color mode

  // Hardware window
  {0x11, 0x01},		//PCLK settings, 15fps
  {0x32, 0x80},		//HREF
  {0x17, 0x17},		//HSTART
  {0x18, 0x05},		//HSTOP
  {0x03, 0x0a},		//VREF
  {0x19, 0x02},		//VSTART
  {0x1a, 0x7a},		//VSTOP

  // Scalling numbers
  {0x70, 0x3a},		//X_SCALING
  {0x71, 0x35},		//Y_SCALING
  {0x72, 0x11},		//DCW_SCALING
  {0x73, 0xf0},		//PCLK_DIV_SCALING
  {0xa2, 0x02},		//PCLK_DELAY_SCALING

  // Matrix coefficients
  {0x4f, 0x80},
  {0x50, 0x80},
  {0x51, 0x00},
  {0x52, 0x22},
  {0x53, 0x5e},
  {0x54, 0x80},
  {0x58, 0x9e},

  // Gamma curve values
  {0x7a, 0x20},
  {0x7b, 0x10},
  {0x7c, 0x1e},
  {0x7d, 0x35},
  {0x7e, 0x5a},
  {0x7f, 0x69},
  {0x80, 0x76},
  {0x81, 0x80},
  {0x82, 0x88},
  {0x83, 0x8f},
  {0x84, 0x96},
  {0x85, 0xa3},
  {0x86, 0xaf},
  {0x87, 0xc4},
  {0x88, 0xd7},
  {0x89, 0xe8},

  // AGC and AEC parameters
  {0xa5, 0x05},
  {0xab, 0x07},
  {0x24, 0x95},
  {0x25, 0x33},
  {0x26, 0xe3},
  {0x9f, 0x78},
  {0xa0, 0x68},
  {0xa1, 0x03},
  {0xa6, 0xd8},
  {0xa7, 0xd8},
  {0xa8, 0xf0},
  {0xa9, 0x90},
  {0xaa, 0x94},
  {0x10, 0x00},

  // AWB parameters
  {0x43, 0x0a},
  {0x44, 0xf0},
  {0x45, 0x34},
  {0x46, 0x58},
  {0x47, 0x28},
  {0x48, 0x3a},
  {0x59, 0x88},
  {0x5a, 0x88},
  {0x5b, 0x44},
  {0x5c, 0x67},
  {0x5d, 0x49},
  {0x5e, 0x0e},
  {0x6c, 0x0a},
  {0x6d, 0x55},
  {0x6e, 0x11},
  {0x6f, 0x9f},
  {0x6a, 0x40},
  {0x01, 0x40},
  {0x02, 0x60},
  {0x13, 0xe7},

  // Additional parameters
  {0x34, 0x11},
  {0x3f, 0x00},
  {0x75, 0x05},
  {0x76, 0xe1},
  {0x4c, 0x00},
  {0x77, 0x01},
  {0xb8, 0x0a},
  {0x41, 0x18},
  {0x3b, 0x12},
  {0xa4, 0x88},
  {0x96, 0x00},
  {0x97, 0x30},
  {0x98, 0x20},
  {0x99, 0x30},
  {0x9a, 0x84},
  {0x9b, 0x29},
  {0x9c, 0x03},
  {0x9d, 0x4c},
  {0x9e, 0x3f},
  {0x78, 0x04},
  {0x0e, 0x61},
  {0x0f, 0x4b},
  {0x16, 0x02},
  {0x1e, 0x00},
  {0x21, 0x02},
  {0x22, 0x91},
  {0x29, 0x07},
  {0x33, 0x0b},
  {0x35, 0x0b},
  {0x37, 0x1d},
  {0x38, 0x71},
  {0x39, 0x2a},
  {0x3c, 0x78},
  {0x4d, 0x40},
  {0x4e, 0x20},
  {0x69, 0x00},
  {0x6b, 0x3a},
  {0x74, 0x10},
  {0x8d, 0x4f},
  {0x8e, 0x00},
  {0x8f, 0x00},
  {0x90, 0x00},
  {0x91, 0x00},
  {0x96, 0x00},
  {0x9a, 0x00},
  {0xb1, 0x0c},
  {0xb2, 0x0e},
  {0xb3, 0x82},
  {0x4b, 0x01},
};


/**
* @brief    Delay
* @param    delay time
* @retval   none
*/
void Delay(volatile uint32_t nCount)
{
  while (nCount--);
}


/**
* @brief  MCO1Init
* @param    none
* @retval   none
*/
void MCO1Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_ClockSecuritySystemCmd(ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // GPIO config
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		//PA8 - XCLK
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // GPIO AF config
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

  // MCO clock source
  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_5);//2
}



/**
  * @brief  Initializes the MCO1 (pin PA8) as XCLK for OV5642 camera module
  * @note	HSI (16MHz) is used as clock source for MCO1 directly (without prescaler)
  * @param  None
  * @retval ERROR if HSI startup is failed
  * 		SUCESS if HSI startup is correct
  */
uint8_t OV5642XCLKHwInit(void)
{
  __IO uint32_t startupcounter = 0;
  uint8_t status = 0;
  FlagStatus HSI_Status = RESET;
  GPIO_InitTypeDef  GPIO_InitStructure;

  // Enable high speed internal 16MHz oscillator *******************************
  RCC_HSICmd(ENABLE);

  // Wait till HSI is ready and if Time out is reached exit
  do
  {
    HSI_Status = RCC_GetFlagStatus(RCC_FLAG_HSIRDY);
    startupcounter++;
  }
  while((startupcounter != DCMI_TIMEOUT_MAX) && (HSI_Status == RESET));

  if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)
    status = 0;
  else
    status = 0xFF;

  // Output HSI clock on MCO1 pin(PA8 - zelena) ****************************************
  // Enable the GPIOA peripheral
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Connect MCO1 pin to AF0
  // Connect to AF0 is default after reset
  // GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

  // Configure MCO1 pin(PA8) in alternate function
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // PLLCLK clock selected to output on MCO1 pin(PA8)
  RCC_MCO1Config(RCC_MCO1Source_HSI, RCC_MCO1Div_1);  //RCC_MCO1Source_PLLCLK
  printf("\r\nXCLK >%dMHz\r\n",(HSI_VALUE/1000000/1));  //RCC_MCO1Div_2
  return(status);
}



/**
* @brief  SSCBInit
* @param    none
* @retval   none
*/
void SCCBInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  // GPIO config
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;		//PB8 - SIOC
  //PB9 - SIOD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // GPIO AF config
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

  // I2C config
  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;//30000;
  I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
  I2C_Init(I2C1, &I2C_InitStructure);
  I2C_Cmd(I2C1, ENABLE);
}



/**
* @brief  Configures the OV7670 camera module configuration over I2C aka SCCB.
* @param  addres of camera data, data
* @retval status of comunication, 0=No error
*/
int i2cSendData(uint8_t regAddr, uint8_t data)
{
  int timeout = I2C_TIMEOUT;
  int ret = 0;

  /* send i2c*/
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((timeout--) == 0)
    {
      ret = 1;
      goto exit;
    }
  }
  timeout = I2C_TIMEOUT;
  I2C_Send7bitAddress(I2C1, OV7670_WRITE_ADDR, I2C_Direction_Transmitter);
  while (!(I2C1->SR1 & (1 << 1))) // check ADDR bit
  {
    if ((timeout--) == 0)
    {
      ret = 2;
      goto exit;
    }
  }
  timeout = I2C_TIMEOUT;
  while (!(I2C1->SR2 & (1 << 2)))   // check TRA bit
  {
    if ((timeout--) == 0)
    {
      ret = 3;
      goto exit;
    }
  }
  timeout = I2C_TIMEOUT;
  I2C_SendData(I2C1, (regAddr));
  while (!(I2C1->SR1 & (1 << 7)))
  {
    if ((timeout--) == 0)
    {
      ret = 4;
      goto exit;
    }
  }

  timeout = I2C_TIMEOUT + I2C_TIMEOUT;
  I2C_SendData(I2C1, data);

  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((timeout--) == 0)
    {
      ret = 5;
      goto exit;
    }
  }
exit:
  I2C_GenerateSTOP(I2C1, ENABLE);

  return ret;
}


/**
* @brief  Read data from OV7670 camera module over I2C aka SCCB.
* @param  address of camera data, data
* @retval status of comunication
*/

int i2cReadData(uint8_t regAddr, uint8_t* data)
{
  int timeout = 0x7FFF;
  int ret = 0;

  /* send i2c*/
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    if ((timeout--) == 0)
    {
      ret = 11;
      goto exit;
    }
  }
  timeout = 0x7FFF;
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((timeout--) == 0)
    {
      ret = 1;
      goto exit;
    }
  }
  timeout = 0x7FFF;
  I2C_Send7bitAddress(I2C1, OV7670_READ_ADDR, I2C_Direction_Transmitter);
  while (!(I2C1->SR1 & (1 << 1)))    // check ADDR bit
  {
    if ((timeout--) == 0)
    {
      ret = 2;
      goto exit;
    }
  }
  timeout = 0x7FFF;
  while (!(I2C1->SR2 & (1 << 2)))   // check TRA bit
  {
    if ((timeout--) == 0)
    {
      ret = 3;
      goto exit;
    }
  }

  timeout = 0x7FFF;
  I2C_SendData(I2C1, (regAddr));
  while (!(I2C1->SR1 & (1 << 7)))
  {
    if ((timeout--) == 0)
    {
      ret = 4;
      goto exit;
    }
  }

  timeout = 0x7FFF;
  I2C_GenerateSTOP(I2C1, ENABLE);
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((timeout--) == 0)
    {
      ret = 5;
      goto exit;
    }
  }
  timeout = 0x7FFF;
  I2C_Send7bitAddress(I2C1, OV7670_READ_ADDR, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // check ADDR bit
  {
    if ((timeout--) == 0)
    {
      ret = 6;
      goto exit;
    }
  }

  timeout = 0x7FFF;
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);

  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    if ((timeout--) == 0)
    {
      ret = 10;
      goto exit;
    }
  }

  *data = I2C_ReceiveData(I2C1);
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  return ret;

exit:
  I2C_GenerateSTOP(I2C1, ENABLE);
  return ret;
}


/**
* @brief  OV7670 camera module init
* @param  none
* @retval status of init
*/
int OV7670Init(void)
{
  uint8_t data, i,j, readData,pid = 0,ver = 0;
  int err = 0;
  char str[] = "00%";
  int tmp;

  //	{0x12, 0x80},		//Reset registers
  data = 0x80;
  Delay(0xFFFFF);
  //err = i2cSendData(0x12, data);

  Delay(0xFFFFF);
  Delay(0xFFFFF);
  //LCD_ILI9341_DrawRectangle(99, 91, 221, 117, ILI9341_COLOR_BLACK);
  //LCD_ILI9341_DrawRectangle(99, 91, 221, 117, ILI9341_COLOR_BLACK);
  //LCD_ILI9341_DrawRectangle(99, 91, 221, 117, ILI9341_COLOR_BLACK);
  //LCD_ILI9341_DrawRectangle(99, 91, 221, 117, ILI9341_COLOR_WHITE);

  if (0 != err)
  {
    printf("E: Err>%d Reset Fail.\r\n", err);
  }

  i2cReadData(0x0A,&pid);
  i2cReadData(0x0B,&ver);

  if((pid == 0x76) && (ver == 0x73))
    printf("I: Find OV%X%X. Configuring...\r\n",pid,ver);
  else
  {
    printf("E: OV7670 not found. Find OV%X%X. Aborting...\r\n",pid,ver);
    return ENXIO;
  }

  // Configure camera registers
  for (i = 0; i < OV7670_REG_NUM_QVGA; i++)
  {
    sprintf(str,"%.02d%%",(i*100)/OV7670_REG_NUM_QVGA);
    Delay(0x00000FFF);
    data = OV7670_regQVGA[i][1];
    err = i2cSendData(OV7670_regQVGA[i][0], data);

    if (0 != err)
    {
      //i2cReadData(OV7670_regQVGA[i][0],&readData);
      printf("E: Err>%d i>%d CAMAdr>%d readData>%d writeData>%d\r\n", err, i, OV7670_regQVGA[i][0], readData, data);
      j = i;
      //LCD_ILI9341_DrawLine(99 + i, 91, 99 + i, 117, ILI9341_COLOR_RED);
      //LCD_ILI9341_Puts(145, 95, str, &LCD_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK);

      //while (j++ < OV7670_REG_NUM_QVGA)
      //  LCD_ILI9341_DrawLine(99 + j, 95, 99 + j, 115, ILI9341_COLOR_RED);

      //return err;
    }
    else
    {
      //LCD_ILI9341_DrawLine(99 + i, 91, 99 + i, 117, ILI9341_COLOR_WHITE);
      //LCD_ILI9341_Puts(145, 95, str, &LCD_Font_11x18, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
    }

  }
  return 0;  //0 = OK, otherwise = fail
}



/**
* @brief    DCMI and DMA init for camera OV7670 data transfer
* @param    none
* @retval   none
*/
void DCMIDMAInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DCMI_InitTypeDef DCMI_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  // GPIO config
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;		//PA4 - HREF, PA6 - PCLK
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = /**GPIO_Pin_6 |**/ GPIO_Pin_7;		//PB6 - D5, PB7 - VSYNC
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;		//PC6 - D0
  //PC7 - D1
  //PC8 - D2
  //PC9 - D3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		//PD3 - D5

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;		//PE4 - D4
  //PE5 - D6
  //PE6 - D7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  // GPIO AF config
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI);

  // DCMI config
  DCMI_DeInit();
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;  //DCMI_VSPolarity_High for 0V7670,5642, DCMI_VSPolarity_Low for 0V2640  14.11.2016
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_Init(&DCMI_InitStructure);
  DCMI_JPEGCmd  (ENABLE);
  DCMI_Cmd(ENABLE);

  // DMA config
  DMA_DeInit(DMA2_Stream1);
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&DCMI->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)CPURamBuffer; //SDRAM_IMG;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = IMG_ROWS*IMG_COLUMNS / 2;// *2/4
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream1, ENABLE);

  // DMA interrupt
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



/**
* @brief    memcpy32
* @param    dst,src,len
* @retval   dst
*/
void * memcpy32(void * dst, void const * src, size_t len)
{
  uint32_t * ldst = (uint32_t *)dst;
  uint32_t * lsrc = (uint32_t *)src;
  while (len >= 16)
  {
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;
    *ldst++ = *lsrc++;

    len -= 32;
  }
  return (dst);
}

/**
* @brief  Erase STM429I Discovery SDRAM
* @param  Initial value
* @retval none
*/
void eraseSDRAM(uint8_t data)
{
  uint32_t cnt;
  /* Erase SDRAM memory */
  for (cnt = SDRAM_BANK_ADDR; cnt < (SDRAM_BANK_ADDR + IS42S16400J_SIZE); cnt++)
  {
    *(__IO uint8_t*) (cnt) = (uint8_t)data;
  }
}


/**
* @brief  Init usart1
* @param  Baudrate
* @retval none
*/
void USART1Init(uint32_t baudRate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Enable UART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  /* Connect PA9 to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

  /* Connect PA10 to USARTx_Rx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* USART configuration */
  USART_InitStructure.USART_BaudRate = baudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}


/**
* @brief  Init UART4
* @param  Baudrate
* @retval none
*/
void UART4Init(uint32_t baudRate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

  /* Connect PA0 to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);

  /* Connect PC11 to USARTx_Rx*/
  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Enable the UART4 Interrupt */

  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

  /* USART configuration */
  USART_InitStructure.USART_BaudRate = baudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &USART_InitStructure);


  /* Enable USART */
  USART_Cmd(UART4, ENABLE);
}



/**
* @brief  memcpy32ExpRGB888
* @param  dst, src,len
* @retval none
*/
void memcpy32ExpRGB888(uint8_t *ldst, uint16_t *lsrc, size_t len)
{
  uint16_t tmp;
  uint8_t r, g, b;

  while ((len--))
  {
    //RGB565 to RGB888
    tmp = *lsrc++;   //MSB LSB in STM32F429i
    r = (((tmp & 0xF800) >> 8));   //8
    g = (((tmp & 0x07E0) >> 3));   //3
    b = (((tmp & 0x001F) << 3));   //3

    *ldst++ = r;
    *ldst++ = g;
    *ldst++ = b;
  }

  //printf("I:memcpy_>%p %p %p 0x%x >%dms\r\n", ldst, dst, src, ldst - (uint8_t *)dst,gTime);
  return;
}
