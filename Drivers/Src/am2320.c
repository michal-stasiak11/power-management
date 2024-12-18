#include "am2320.h"

static bool am2320_checksum(uint8_t* buffer)
{
	uint16_t checksum_reg = (buffer[7] << 8) | buffer[6];
	uint16_t checksum = 0xFFFF;

	for(uint8_t i = 0; i < 6; i++)
	{
		checksum ^= buffer[i];
		for(uint8_t j = 0; j < 8; j++)
		{
			if(checksum & 0x1)
			{
				checksum >>= 1;
				checksum ^= 0xA001;
			}
			else
			{
				checksum >>= 1;
			}
		}
	}

	return (checksum_reg == checksum);
}

HAL_StatusTypeDef am2320_data_fetch(am2320_config_t const* config)
{
	HAL_Delay(2000);
	HAL_I2C_Master_Transmit(config->i2c, config->address, AM2320_WAKEUP, 0, config->timeout);
	HAL_Delay(1);
	uint8_t registers[3] = {AM2320_READ, AM2320_HUMIDITY, AM2320_TEMP};
	if (HAL_I2C_Master_Transmit(config->i2c, config->address, registers, 3, config->timeout) != HAL_OK)
	{
		return HAL_ERROR;
	}
	HAL_Delay(2);
	if (HAL_I2C_Master_Receive(config->i2c, config->address, config->buffer, 8, config->timeout) != HAL_OK)
	{
		return HAL_ERROR;
	}
	if ((config->buffer[0] != AM2320_READ) || (config->buffer[1] != 4))
	{
		return HAL_ERROR;
	}
	return am2320_checksum(config->buffer) ? HAL_OK : HAL_ERROR;
	return HAL_OK;
}

float am2320_temp_get(am2320_config_t const* config)
{
	uint16_t temperature = (config->buffer[4] << 8) | config->buffer[5];
	int16_t signed_temperature;
	if(temperature & 0x8000)
	{
		signed_temperature = -1 * (int16_t)(temperature & 0x7fff);
	}
	else
	{
		signed_temperature = (int16_t)temperature;
	}
	return (float)signed_temperature / 10.0;
}

float am2320_humidity_get(am2320_config_t const* config)
{
	uint16_t humidity = (config->buffer[2] << 8) | config->buffer[3];
	return (float)humidity / 10.0;
}

