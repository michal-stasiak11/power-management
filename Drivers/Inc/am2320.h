/*
 * am2320.h
 *
 *  Created on: Oct 13, 2024
 *      Author: Admin
 */

#ifndef INC_AM2320_H_
#define INC_AM2320_H_

#include "stm32l4xx.h"
#include <stdbool.h>

#define AM2320_WAKEUP 	0x00
#define AM2320_READ     0x03

#define AM2320_HUMIDITY 0x00
#define AM2320_TEMP     0x04

typedef struct
{
    I2C_HandleTypeDef* i2c;
    uint32_t           timeout;
    uint16_t           address;
    uint8_t*		   buffer;
} am2320_config_t;

HAL_StatusTypeDef am2320_data_fetch(am2320_config_t const* config);

float am2320_temp_get(am2320_config_t const* config);

float am2320_humidity_get(am2320_config_t const* config);

#endif /* INC_AM2320_H_ */
