/*
 * lps25hb.h
 *
 *  Created on: Oct 10, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_LPS25HB_H_
#define INC_LPS25HB_H_

#include "stm32l4xx.h"
#include <stdbool.h>
#include <stdio.h>

#define LPS25HB_I2C_ENABLED 0
#define LPS25HB_SPI_ENABLED 1

#if LPS25HB_I2C_ENABLED
typedef struct
{
    I2C_HandleTypeDef* i2c;
    uint16_t           address;
    uint16_t           timeout;
    uint16_t 		   calib;
} lps25hb_config_t;
#elif LPS25HB_SPI_ENABLED
typedef struct
{
	SPI_HandleTypeDef* spi;
    GPIO_TypeDef *     port;
    uint16_t           pin;
    uint16_t           timeout;
    uint16_t 		   calib;
} lps25hb_config_t;
#else
#error "No communication protocol provided"
#endif

typedef enum
{
    LPS25HB_WHO_AM_I  = 0x0F,
    LPS25HB_CTRL_REG1 = 0x20,
    LPS25HB_CTRL_REG2 = 0x21,
    LPS25HB_PRESSURE  = 0xA8,
    LPS25HB_TEMP      = 0xAB,
    LPS25HB_RPDS_L 	  = 0x39,
    LPS25HB_RPDS_H 	  = 0x3A,
    LPS25HB_FIFO_CTRL = 0x2E
} lps25hb_reg_t;

typedef enum
{
    LPS25HB_WHO_AM_I_SUCCESS      = 0xBD,
    LPS25HB_CTRL_REG1_WAKEUP      = 0x80,
    LPS25HB_CTRL_REG1_SLEEP       = 0x00,
	LPS25HB_CTRL_REG2_FIFO_ENABLE = 0x01,
	LPS25HB_FIFO_CTRL_MEAN        = 0xC0,
	LPS25HB_FIFO_CTRL_WTM_32      = 0x1F,
	LPS25HB_SPI_READ_INCREMENT    = 0xC0,
	LPS25HB_SPI_READ_NOINCREMENT  = 0x80
} lps25hb_val_t;

HAL_StatusTypeDef lps25hb_init(lps25hb_config_t const* config);

float lps25hb_temp_read(lps25hb_config_t const* config);

float lps25hb_pressure_read(lps25hb_config_t const* config);

void lps25hb_uninit(lps25hb_config_t const* config);

#endif /* INC_LPS25HB_H_ */
