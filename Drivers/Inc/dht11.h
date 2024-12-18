/*
 * dht11.h
 *
 *  Created on: Sep 26, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32l4xx.h"

typedef struct
{
	TIM_HandleTypeDef *  timer;
    GPIO_TypeDef * 	     port;
    uint16_t             pin;
    uint8_t *            buffer;
}   dht11_config_t;

HAL_StatusTypeDef dht11_data_fetch(dht11_config_t const* config);

uint8_t dht11_temp_get(dht11_config_t const* config);

uint8_t dht11_humidity_get(dht11_config_t const* config);

#endif /* INC_DHT11_H_ */
