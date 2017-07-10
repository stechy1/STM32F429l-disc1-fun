/*
*	==========================================================================
*   main.c
*
*   Description:
*   Camera sensor system main file.
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
*	==========================================================================
*/

/**
* @brief  Defines
* @param  None
* @retval None
*/
//---MDK-ARM---
//429ZI
//STM32F429_439xx,USE_STDPERIPH_DRIVER,STM32F4XX,__ASSEMBLY__,
//KEIL_IDE,TM_DISCO_STM32F429_DISCOVERY,USE_USB_OTG_HS,HSE_VALUE=8000000

//---EMBitz---
//ARM_MATH_CM4
//__FPU_USED
//STM32F429ZI
//STM32F4XX
//USE_STDPERIPH_DRIVER
//STM32F429_439xx
//HSE_VALUE=8000000
//USE_USB_OTG_HS


#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_rcc.h"

//USB
#include "usb_cdc_device/usbd_cdc_vcp.h"
#include "usb_cdc_device/usbd_conf.h"
#include "usb_cdc_device/usbd_usr.h"
#include "usb_cdc_device/usbd_core.h"
#include "usb_cdc_device/usbd_desc.h"
//#include "vdclass.h"

//misc
#include "OV7670_control.h"
#include "lcd_ili9341.h"
#include "lcd_fonts.h"
#include "lcd_spi.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery.h"
#include "savejpg.h"
#include "rtc.h"

//#define ADC1_CDR_Address    ((uint32_t)0x40012308)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

#define DEBUG

#ifdef DEBUG
#define dbgPrint printf
#else
#define dbgPrint(...) /*...*/
#endif

#define PC_REQ_DATA 33

static __IO bool gFrameReady = false;
static uint8_t gCnt = 4;
__IO bool gStartTransfer = false;
__IO uint32_t gTime = 0;
uint8_t* gPtrJPEGImg = (uint8_t*) SDRAM_JPEG;
uint8_t* gPtrRGB888Img = (uint8_t*) SDRAM_RGB888;
uint8_t gChar = 0;
uint8_t Buffer[255] = {0};
extern uint8_t seqNo;

uint8_t CPURamBuffer[IMG_LENGHT] = {0};
uint8_t CPUJpeg[25000] = {0};

///**
//* @brief  Retargets the C library printf function to the i.e. USART gnu gcc
//* @param  None
//* @retval None
//*/
//int _write(int file, char *ptr, int len)
//{
//  int n;
//  int ch;
//  uint16_t timeout = 0xffff;
//  //return len;
//  for (n = 0; n < len; n++)
//  {
//    ch = *ptr++ & (uint8_t)0xFF;
//    while ((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET) && (timeout--));
//    USART_SendData(UART4, ch);
//  }
//  return len;
//}


/**
* @brief  Retargets the C library printf function to the i.e. USART. mdk-arm
* @param  None
* @retval None
*/
int fputc(int ch, FILE *f)
{
  uint16_t timeout = 0xffff;

  /* write a character to the USART */
  while ((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET) && (timeout--));
  USART_SendData(UART4, (uint8_t)ch);
  return ch;
}


/**
* @brief  Send image via USB VCP, RS232, USB bulk
* @param  img pointer
* @retval status, 0 = No error
*/
int VCP_SendImage(uint8_t *image)
{
  int j;
  int err = 0;


#define WIDTH         480
#define HEIGHT        640

#define LEN_VCP			384
#define LEN_VCP_MAX	16384
#define LEN_WIFI		2035
#define LEN_UART	  1


#define LEN     LEN_VCP

  gPtrJPEGImg = (uint8_t*) image;

  for (j = 0; j < IMG_LENGHT; gPtrJPEGImg += LEN, j += LEN)
  {
    //while ((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET));  //USARTx
    //USART_SendData(UART4, *gPtrJPEGImg); // USARTx

    VCP_DataTx(gPtrJPEGImg, LEN);  //USB VCP
  }
  return err;
}

/**
* @brief  main fce
* @param  None
* @retval None
*/
int main(void)
{
  int res = 0,err,cnt;         // !=0 fail,0 = OK,
  unsigned char c;
  bool flagSend = false;
  RTC_TimeTypeDef time;
  char str[] = "00:00:00";
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
  char strWait[] = "Waiting for USB...";
  uint32_t start;
  RCC_ClocksTypeDef RCC_Clocks;

  SystemInit(); // System init
  RCC_GetClocksFreq(&RCC_Clocks);

  if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 100))
    while(1);// Capture error

  initRTC();    //Init RTC time to compilation time
  sprintf(str,"%s",__TIME__);
  time.RTC_Hours = atoi(str);
  time.RTC_Minutes = atoi(&str[3]);
  time.RTC_Seconds =  atoi(&str[6]);
  RTC_SetTime(RTC_Format_BCD, &time);

  //hw inits
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  UART4Init(921600);
  USART1Init(921600);
  MCO1Init();
  //OV5642XCLKHwInit();
  LCD_ILI9341_Init();
  DCMIDMAInit();
  SCCBInit();

  err = 0;
  // LCD start page
  LCD_ILI9341_Rotate(LCD_ILI9341_Orientation_Landscape_2);
  LCD_ILI9341_Fill(ILI9341_COLOR_BLACK);
  dbgPrint("I: Build info >"__FILE__ " "__DATE__" "__TIME__" %d %dMHz\r\n", __LINE__,SystemCoreClock/1000000);

  if (0 != err)
    LCD_ILI9341_Puts(40, 15, "HW config fail.", &LCD_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK);
  else
    LCD_ILI9341_Puts(40, 15, "HW config done.", &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);

  LCD_ILI9341_Puts(2, 132, __DATE__, &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
  LCD_ILI9341_Puts(185, 132, __TIME__, &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
  LCD_ILI9341_Puts(20, 55, "Configuring camera", &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);

  cnt = 10;
  while(cnt--)
  {
    err = OV7670Init();    // OV7670 configuration
    if (0 == err)
      break;

    delay_ms(500);
  }

  if (0 != err)
  {
    LCD_ILI9341_Puts(100, 160, "Failed", &LCD_Font_16x26, ILI9341_COLOR_RED, ILI9341_COLOR_BLACK);
    while (1)
    {
      LCD_ILI9341_Puts(20, 210, "Push reset button", &LCD_Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
      dbgPrint("E: Camera config fail.\r\n");
      cnt = 0x0FFF;
      while (cnt-- > 0);
      LCD_ILI9341_Puts(20, 210, "Push reset button", &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
    }
  }
  else
  {
    dbgPrint("I: Camera config done.\r\n");
    LCD_ILI9341_Puts(100, 160, "Success", &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
  }

  // Get the RTC current Time
  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
  // Get the RTC current Date
  RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);

  // Display time Format : hh:mm:ss
  dbgPrint("I: %0.2d:%0.2d:%0.2d ",(RTC_TimeStructure.RTC_Hours), (RTC_TimeStructure.RTC_Minutes),( RTC_TimeStructure.RTC_Seconds));
  dbgPrint("HW config done.\r\n");

//*--- Init USB VCP &USBD_CDC_cb or bulk &USBD_VD_cb

  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
            USB_OTG_HS_CORE_ID,
#else
            USB_OTG_FS_CORE_ID,
#endif
            &USR_desc,
            &USBD_CDC_cb,
            &USR_cb);
//---*/

  while (USB_OTG_dev.dev.device_status != USB_OTG_CONFIGURED)   //cdc,bulk
  {
    LCD_ILI9341_Puts(20, 200, strWait, &LCD_Font_16x26, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK);
    LCD_ILI9341_Puts(20, 200, strWait, &LCD_Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
  }

  DCMI_CaptureCmd(DISABLE);
  DCMI_CaptureCmd(ENABLE);
  LCD_ILI9341_Rotate(LCD_ILI9341_Orientation_Landscape_2);
  STM_EVAL_LEDOff(LED3); //GREEN
  STM_EVAL_LEDOff(LED4); //RED

  gPtrJPEGImg = (uint8_t*) SDRAM_JPEG;
  initJpg();

  // Infinite program loop
  while (1)
  {
    if (true == gFrameReady)
    {
      if (true == gStartTransfer)
      {
        STM_EVAL_LEDOn(LED3); //GREEN
        DMA_Cmd(DMA2_Stream1, DISABLE);
        DCMI_CaptureCmd(DISABLE);
        VCP_SendImage(CPURamBuffer);
        DMA_Cmd(DMA2_Stream1, ENABLE);
        DCMI_CaptureCmd(ENABLE);
        gStartTransfer = false;
        STM_EVAL_LEDOff(LED3); //GREEN
      }

      STM_EVAL_LEDOn(LED4); //RED
      //dbgPrint("I:Start send img to LCD >%dms.\r\n",gTime);
      LCD_ILI9341_DisplayImage((uint16_t*)CPURamBuffer);
      //dbgPrint("I:End send img to LCD >%dms.\r\n",gTime);
      STM_EVAL_LEDOff(LED4);  //RED
      gFrameReady = false;
    }

//If something arrived at VCP
    if (usb_cdc_data_available() == 1)
    {
      c = usb_cdc_getc();
      dbgPrint("I: New data from VCP >%d\r\n",c);
      if (PC_REQ_DATA == c) gStartTransfer = true;

      c = 0;
    }
  }
}



/**
* @brief  This function handles DMA2_Stream1 global interrupt request.
* @param  None
* @retval None
*/
void DMA2_Stream1_IRQHandler(void)
{
  // DMA complete
  if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) != RESET)
  {
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);

    DMA_Cmd(DMA2_Stream1, ENABLE);
    gFrameReady = true;
    gCnt++;
    //dbgPrint("I:DMA >%d %dms\r\n",gCnt,gTime);
  }
}


/**
* @brief  This function handles UART4 global interrupt request.
* @param  None
* @retval None
*/

void UART4_IRQHandler(void)
{
  if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
  {
    //STM_EVAL_LEDToggle(LED3); //GREEN
    gChar = USART_ReceiveData(UART4);
    dbgPrint("I:USART4 Received >%c >%03d\r\n", gChar, (uint8_t)gChar);
    if (gChar == PC_REQ_DATA) gStartTransfer = true;  // "!" to start transfer
    if (gChar == 'R') NVIC_SystemReset();     // "R" to Reset
  }
}


/**
* @brief  This function handles USART1 global interrupt request.
* @param  None
* @retval None
*/

void USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    STM_EVAL_LEDToggle(LED3); //GREEN
    gChar = USART_ReceiveData(USART1);
    dbgPrint("I:USART1 Received >%c >%03d\r\n", gChar, (uint8_t)gChar);
    if (gChar == PC_REQ_DATA) gStartTransfer = true;  // "!" to start transfer
    if (gChar == 'R') NVIC_SystemReset();     // "R" to Reset
  }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  gTime += 10;
}
