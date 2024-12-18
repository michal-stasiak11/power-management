#include "dht11.h"
#include <stdio.h>

static void delay_us(TIM_HandleTypeDef* timer, uint32_t us)
{
	__HAL_TIM_SET_COUNTER(timer, 0);
	while (__HAL_TIM_GET_COUNTER(timer) < us) {}
}

static HAL_StatusTypeDef dht11_reset(dht11_config_t const* config)
{
	HAL_TIM_Base_Start(config->timer);
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_SET);
    delay_us(config->timer, 30);
    uint8_t tries = 0;
    while ((HAL_GPIO_ReadPin(config->port, config->pin) == GPIO_PIN_SET) && (tries < 100))
    {
    	tries++;
    	delay_us(config->timer, 1);
    }
    if(tries >= 100)
    {
    	return HAL_ERROR;
    }
    tries = 0;
    while ((HAL_GPIO_ReadPin(config->port, config->pin) == GPIO_PIN_RESET) && (tries < 100))
    {
    	tries++;
    	delay_us(config->timer, 1);
    }
    if(tries >= 100)
    {
    	return HAL_ERROR;
    }
    return HAL_OK;
}

static uint8_t dht11_bit_read(dht11_config_t const* config)
{   

 	uint8_t tries = 0;
    while ((HAL_GPIO_ReadPin(config->port, config->pin) == GPIO_PIN_SET) && (tries < 100))
    {
    	tries++;
    	delay_us(config->timer, 1);
    }
    tries = 0;
    while ((HAL_GPIO_ReadPin(config->port, config->pin) == GPIO_PIN_RESET) && (tries < 100))
    {
    	tries++;
    	delay_us(config->timer, 1);
    }
	delay_us(config->timer, 40);
	return (HAL_GPIO_ReadPin(config->port, config->pin) == GPIO_PIN_SET) ? 1 : 0;
}   

static uint8_t dht11_byte_read(dht11_config_t const* config)
{
    uint8_t byte = 0;
    for(uint8_t bit = 0; bit < 8; bit++)
    {
    	byte <<= 1;
        byte |= dht11_bit_read(config);
    }
    return byte;
}

static HAL_StatusTypeDef dht11_checksum(dht11_config_t const* config)
{
    return (config->buffer[0] +
            config->buffer[1] +
            config->buffer[2] +
            config->buffer[3] ==
            config->buffer[4]) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef dht11_data_fetch(dht11_config_t const* config)
{
    if(dht11_reset(config) == HAL_ERROR)
    {
        return HAL_ERROR;
    }
    for(uint8_t i = 0; i < 5; i++)
    {
        config->buffer[i] = dht11_byte_read(config);
    }
    return dht11_checksum(config);
}

uint8_t dht11_temp_get(dht11_config_t const* config)
{
    return config->buffer[2];
}

uint8_t dht11_humidity_get(dht11_config_t const* config)
{
    return config->buffer[0];
}
