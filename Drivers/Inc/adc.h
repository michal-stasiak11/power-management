/*
 * adc.h
 *
 *  Created on: Aug 26, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"

typedef float (*adc_sensor_converter_t)(uint16_t value, uint8_t resolution);

typedef struct
{
    ADC_HandleTypeDef * adc;
    uint16_t *    buffer;
    uint8_t       resolution;
    uint8_t       channels;
} adc_config_t;

typedef struct
{
    adc_config_t const *   adc_config;
    adc_sensor_converter_t converter;
    uint8_t                channel;
} adc_sensor_config_t;

void adc_values_poll(adc_config_t const* config);

float adc_sensor_value_get(adc_sensor_config_t const* config);

#endif /* INC_ADC_H_ */
