/*
 * gmouse_lld_STMPE811_board.h
 *
 *  Created on: 21. 12. 2017
 *      Author: Petr
 */

#ifndef GMOUSE_LLD_STMPE811_BOARD_H_
#define GMOUSE_LLD_STMPE811_BOARD_H_

#include "../Drivers/BSP/Components/stmpe811/stmpe811.h"

// Resolution and Accuracy Settings
#define GMOUSE_STMPE811_PEN_CALIBRATE_ERROR		8
#define GMOUSE_STMPE811_PEN_CLICK_ERROR			6
#define GMOUSE_STMPE811_PEN_MOVE_ERROR			4
#define GMOUSE_STMPE811_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_STMPE811_FINGER_CLICK_ERROR		18
#define GMOUSE_STMPE811_FINGER_MOVE_ERROR		14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_STMPE811_BOARD_DATA_SIZE			0

// Options - Leave these commented to make it user configurable in the gfxconf.h
//#define GMOUSE_STMPE811_READ_PRESSURE		FALSE
//#define GMOUSE_STMPE811_SELF_CALIBRATE	FALSE
//#define GMOUSE_STMPE811_TEST_MODE			FALSE

// If TRUE this board has the STMPE811 IRQ pin connected to a GPIO.
// Note: For tested hardware this is unreliable and should be set to FALSE until tested.
//			Symptoms are that mouse readings just appear to stop for a bit. Lifting the touch
//			and re-applying the touch cause readings to start again.
#define GMOUSE_STMPE811_GPIO_IRQPIN				FALSE

// If TRUE this is a really slow CPU and we should always clear the FIFO between reads.
#define GMOUSE_STMPE811_SLOW_CPU				FALSE

#define I2C_MEMADD_SIZE_8BIT            0x00000001U

// Slave address
#define STMPE811_ADDR				0x41

// Maximum timeout
#define STMPE811_TIMEOUT			0x3000


extern I2C_HandleTypeDef hi2c3;

static bool_t init_board(GMouse* m, unsigned driverinstance) {
	//stmpe811_Init(driverinstance);

	return TRUE;
}

#if GMOUSE_STMPE811_GPIO_IRQPIN
	static bool_t getpin_irq(GMouse* m) {

	}
#endif

static GFXINLINE void aquire_bus(GMouse* m) {
}

static GFXINLINE void release_bus(GMouse* m) {
}

static void write_reg(GMouse* m, uint8_t reg, uint8_t val) {
	uint8_t txbuf[2];
	//IOE_Write(STMPE811_ADDR, reg, val);
	HAL_StatusTypeDef status = HAL_OK;
	txbuf[0] = reg;
	txbuf[1] = val;

	 status = HAL_I2C_Mem_Write(&hi2c3, (uint16_t)STMPE811_ADDR<<1, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, &val, 1, STMPE811_TIMEOUT);

	  /* Check the communication status */
	  if(status != HAL_OK)
	  {
	    /* Re-Initialize the BUS */
	    //I2Cx_Error();
		  _Error_Handler(__FILE__, __LINE__);
	  }
}

static uint8_t read_byte(GMouse* m, uint8_t reg) {
	//return IOE_Read(STMPE811_ADDR, reg);
	  HAL_StatusTypeDef status = HAL_OK;
	  uint8_t value = 0;

	  status = HAL_I2C_Mem_Read(&hi2c3, (uint16_t)STMPE811_ADDR<<1, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, STMPE811_TIMEOUT);

	  /* Check the communication status */
	  if(status != HAL_OK)
	  {
	    /* Re-Initialize the BUS */
	    //I2Cx_Error();
		  _Error_Handler(__FILE__, __LINE__);

	  }
	  return value;

}

static uint16_t read_word(GMouse* m, uint8_t reg) {

}


#endif /* GMOUSE_LLD_STMPE811_BOARD_H_ */
