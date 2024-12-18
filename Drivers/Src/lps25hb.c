#include "lps25hb.h"
#include <stdio.h>

#if LPS25HB_I2C_ENABLED
static uint32_t lps25hb_reg_read(lps25hb_config_t const* config, lps25hb_reg_t reg, uint8_t bytes)
{
    uint32_t value = 0;
    HAL_I2C_Mem_Read(config->i2c, config->address, reg, 1, (uint8_t*)&value, bytes, config->timeout);

    return value;
}

static void lps25hb_reg_write(lps25hb_config_t const* config, lps25hb_reg_t reg, uint8_t value)
{
    HAL_I2C_Mem_Write(config->i2c, config->address, reg, 1, &value, 1, config->timeout);
}

HAL_StatusTypeDef lps25hb_init(lps25hb_config_t const* config)
{
	if (lps25hb_i2c_reg_read(config, LPS25HB_WHO_AM_I, 1) != LPS25HB_WHO_AM_I_SUCCESS)
    {
        return HAL_ERROR;
    }
	lps25hb_reg_write(config, LPS25HB_RPDS_L, config->calib);
	lps25hb_reg_write(config, LPS25HB_RPDS_H, config->calib >> 8);
	lps25hb_reg_write(config, LPS25HB_CTRL_REG1, LPS25HB_CTRL_REG1_WAKEUP);
	lps25hb_reg_write(config, LPS25HB_FIFO_CTRL,  LPS25HB_FIFO_CTRL_MEAN | LPS25HB_FIFO_CTRL_WTM_32);
	lps25hb_reg_write(config, LPS25HB_CTRL_REG2, LPS25HB_CTRL_REG2_FIFO_ENABLE);
	return HAL_OK;
}

float lps25hb_temp_read(lps25hb_config_t const* config)
{
     int16_t temp = lps25hb_i2c_reg_read(config, LPS25HB_TEMP, 2);

     return 42.5f + temp / 480.0f;
}

float lps25hb_pressure_read(lps25hb_config_t const* config)
{
     int32_t pressure = lps25hb_reg_read(config, LPS25HB_PRESSURE, 3);

     return pressure / 4096.0f;
}

void lps25hb_uninit(lps25hb_config_t const* config)
{
    lps25hb_i2c_reg_write(config, LPS25HB_CTRL_REG1, LPS25HB_CTRL_REG1_SLEEP);
}
#elif LPS25HB_SPI_ENABLED

static uint32_t lps25hb_reg_read(lps25hb_config_t const* config, lps25hb_reg_t reg, uint8_t bytes, bool increment)
{
    uint32_t value = 0;
    uint8_t address = reg | (increment ? LPS25HB_SPI_READ_INCREMENT : LPS25HB_SPI_READ_NOINCREMENT);
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(config->spi, &address, 1, config->timeout);
    HAL_SPI_Receive(config->spi, (uint8_t*)&value, bytes, config->timeout);
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_SET);
    return value;
}

static void lps25hb_reg_write(lps25hb_config_t const* config, lps25hb_reg_t reg, uint8_t value)
{
	HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(config->spi, &reg, 1, config->timeout);
    HAL_SPI_Transmit(config->spi, &value, 1, config->timeout);
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef lps25hb_init(lps25hb_config_t const* config)
{
	if (lps25hb_reg_read(config, LPS25HB_WHO_AM_I, 1, false) != LPS25HB_WHO_AM_I_SUCCESS)
    {
        return HAL_ERROR;
    }
	lps25hb_reg_write(config, LPS25HB_RPDS_L, config->calib);
	lps25hb_reg_write(config, LPS25HB_RPDS_H, config->calib >> 8);
	lps25hb_reg_write(config, LPS25HB_CTRL_REG1, LPS25HB_CTRL_REG1_WAKEUP);
	lps25hb_reg_write(config, LPS25HB_FIFO_CTRL,  LPS25HB_FIFO_CTRL_MEAN | LPS25HB_FIFO_CTRL_WTM_32);
	lps25hb_reg_write(config, LPS25HB_CTRL_REG2, LPS25HB_CTRL_REG2_FIFO_ENABLE);
	return HAL_OK;
}

float lps25hb_temp_read(lps25hb_config_t const* config)
{
     int16_t temp = lps25hb_reg_read(config, LPS25HB_TEMP, 2, true);

     return 42.5f + temp / 480.0f;
}

float lps25hb_pressure_read(lps25hb_config_t const* config)
{
     int32_t pressure = lps25hb_reg_read(config, LPS25HB_PRESSURE, 3, true);

     return pressure / 4096.0f;
}

void lps25hb_uninit(lps25hb_config_t const* config)
{
    lps25hb_reg_write(config, LPS25HB_CTRL_REG1, LPS25HB_CTRL_REG1_SLEEP);
}

#endif
