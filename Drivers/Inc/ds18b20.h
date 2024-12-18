/*
 * ds18b20.h
 *
 *  Created on: Aug 17, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "stm32l4xx.h"
#include <stdbool.h>

#define DS18B20_BITBANG 0
#define DS18B20_UART 	0

#define DS18B20_ROM_CODE_SIZE	   8

#define DS18B20_SCRATCHPAD_SIZE    9
 
#define DS18B20_ROM_READ           0x33
#define DS18B20_ROM_MATCH          0x55
#define DS18B20_ROM_SKIP           0xCC
 
#define DS18B20_TEMP_CONVERT       0x44
#define DS18B20_SCRATCHPAD_READ    0xBE

#if DS18B20_BITBANG
typedef struct
{
    TIM_HandleTypeDef *  timer;
    GPIO_TypeDef *       port;
    uint16_t             pin;
    uint8_t * 			 rom_code;
} ds18b20_bb_config_t;


HAL_StatusTypeDef ds18b20_bb_init(ds18b20_bb_config_t const* config, bool multi_instance);

HAL_StatusTypeDef ds18b20_bb_sampling_start(ds18b20_bb_config_t const* config);

float ds18b20_bb_temp_get(ds18b20_bb_config_t const* config);
#endif

#if DS18B20_UART
typedef struct
{
    UART_HandleTypeDef * uart;
    USART_TypeDef *      uart_instance;
    uint8_t * 			 rom_code;
} ds18b20_u_config_t;

HAL_StatusTypeDef ds18b20_u_init(ds18b20_u_config_t const* config, bool multi_instance);

HAL_StatusTypeDef ds18b20_u_sampling_start(ds18b20_u_config_t const* config);

float ds18b20_u_temp_get(ds18b20_u_config_t const* config);
#endif

#endif /* INC_DS18B20_H_ */
