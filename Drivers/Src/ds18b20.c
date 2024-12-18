#include <string.h>
#include "ds18b20.h"

static uint8_t byte_checksum(uint8_t crc, uint8_t byte)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        uint8_t b = crc ^ byte;
        crc >>= 1;
        if (b & 0x01) crc ^= 0x8c;
        byte >>= 1;
    }
    return crc;
}

#if DS18B20_BITBANG
static void delay_us(TIM_HandleTypeDef* timer, uint32_t us)
{
    __HAL_TIM_SET_COUNTER(timer, 0);

    while (__HAL_TIM_GET_COUNTER(timer) < us) {}
}

static int bb_bit_read(TIM_HandleTypeDef* timer, GPIO_TypeDef* port, uint16_t pin)
{
    int rc;
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    delay_us(timer, 6);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    delay_us(timer, 9);
    rc = HAL_GPIO_ReadPin(port, pin);
    delay_us(timer, 55);
    return rc;
}

static void bb_bit_write(TIM_HandleTypeDef* timer, GPIO_TypeDef* port, uint16_t pin, int value)
{
    if (value)
    {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
        delay_us(timer, 6);
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
        delay_us(timer, 64);
    } else
    {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
        delay_us(timer, 60);
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
        delay_us(timer, 10);
    }
}

static HAL_StatusTypeDef bb_1wire_reset(ds18b20_bb_config_t const* config)
{
    int rc;
    
    __disable_irq();
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_RESET);
    delay_us(config->timer, 480);
    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_SET);
    delay_us(config->timer, 70);
    rc = HAL_GPIO_ReadPin(config->port, config->pin);
    delay_us(config->timer, 410);
    __enable_irq();
    
    if (rc == 0)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

static uint8_t bb_1wire_read(ds18b20_bb_config_t const* config)
{
    uint8_t value = 0;
    int i;
    for (i = 0; i < 8; i++)
    {
        value >>= 1;
        __disable_irq();
        if (bb_bit_read(config->timer, config->port, config->pin))
        {
            value |= 0x80;
        } 
        __enable_irq();
    }
    return value;
}

static void bb_1wire_write(ds18b20_bb_config_t const* config, uint8_t byte)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        __disable_irq();
        bb_bit_write(config->timer, config->port, config->pin, byte & 0x01);
        byte >>= 1;
        __enable_irq();
    }
}

static uint8_t bb_1wire_checksum(ds18b20_bb_config_t const* config, const uint8_t* data, int len)
{
    int i;
    uint8_t crc = 0;
    for (i = 0; i < len; i++)
    {
        crc = byte_checksum(crc, data[i]);
    }
    return crc;
}

static HAL_StatusTypeDef bb_1wire_cmd_send(ds18b20_bb_config_t const* config, uint8_t cmd)
{
    int i;
    
    if (bb_1wire_reset(config) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (config->rom_code == NULL)
    {
        bb_1wire_write(config, DS18B20_ROM_SKIP);
    } 
    else
    {
        bb_1wire_write(config, DS18B20_ROM_MATCH);
        for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
        bb_1wire_write(config, config->rom_code[i]);
    }

    bb_1wire_write(config, cmd);

    HAL_GPIO_WritePin(config->port, config->pin, GPIO_PIN_RESET);

    return HAL_OK;
}

static HAL_StatusTypeDef bb_1wire_scratchpad_read(ds18b20_bb_config_t const* config, uint8_t* scratchpad)
{
    int i;
    uint8_t crc;
    
    if (bb_1wire_cmd_send(config, DS18B20_SCRATCHPAD_READ) != HAL_OK)
    {
        return HAL_ERROR;
    }

    for (i = 0; i < DS18B20_SCRATCHPAD_SIZE; i++)
    {
        scratchpad[i] = bb_1wire_read(config);
    }

    crc = bb_1wire_checksum(config, scratchpad, DS18B20_SCRATCHPAD_SIZE - 1);

    if (scratchpad[DS18B20_SCRATCHPAD_SIZE - 1] == crc)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

static HAL_StatusTypeDef bb_1wire_address_read(ds18b20_bb_config_t const* config)
{
    int i;
    uint8_t crc;

    if (bb_1wire_reset(config) != HAL_OK)
    {
        return HAL_ERROR;
    }

    bb_1wire_write(config, DS18B20_ROM_READ);

    for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
    {
        config->rom_code[i] = bb_1wire_read(config);
    }

    crc = bb_1wire_checksum(config, config->rom_code, DS18B20_ROM_CODE_SIZE - 1);

    if (config->rom_code[DS18B20_ROM_CODE_SIZE - 1] != crc)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}
#endif

#if DS18B20_UART
static void baudrate_set(UART_HandleTypeDef* uart, USART_TypeDef* uart_instance, uint32_t baudrate)
{
    uart->Instance = uart_instance;
    uart->Init.BaudRate = baudrate;
    uart->Init.WordLength = UART_WORDLENGTH_8B;
    uart->Init.StopBits = UART_STOPBITS_1;
    uart->Init.Parity = UART_PARITY_NONE;
    uart->Init.Mode = UART_MODE_TX_RX;
    uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart->Init.OverSampling = UART_OVERSAMPLING_16;
    uart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
    uart->AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
    if (HAL_HalfDuplex_Init(uart) != HAL_OK)
    {
        while(1){};
    }
}

static int u_bit_read(UART_HandleTypeDef* uart)
{
    uint8_t data_out = 0xFF;
    uint8_t data_in = 0;
    HAL_UART_Transmit(uart, &data_out, 1, HAL_MAX_DELAY);
    HAL_UART_Receive(uart, &data_in, 1, HAL_MAX_DELAY);
    
    return data_in & 0x01;
}

static void u_bit_write(UART_HandleTypeDef* uart, int value)
{
    if (value)
    {
        uint8_t data_out = 0xff;
        HAL_UART_Transmit(uart, &data_out, 1, HAL_MAX_DELAY);
    }
    else
    {
        uint8_t data_out = 0x0;
        HAL_UART_Transmit(uart, &data_out, 1, HAL_MAX_DELAY);
    }
}

static HAL_StatusTypeDef u_1wire_reset(ds18b20_u_config_t const* config)
{
    uint8_t data_out = 0xF0;
    uint8_t data_in = 0;

    baudrate_set(config->uart, config->uart_instance, 9600);
    HAL_UART_Transmit(config->uart, &data_out, 1, HAL_MAX_DELAY);
    HAL_UART_Receive(config->uart, &data_in, 1, HAL_MAX_DELAY);
    baudrate_set(config->uart, config->uart_instance, 115200);

    if (data_in != 0xF0)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

static uint8_t u_1wire_read(ds18b20_u_config_t const* config)
{
    uint8_t value = 0;
    int i;
    for (i = 0; i < 8; i++)
    {
        value >>= 1;
        if (u_bit_read(config->uart))
        value |= 0x80;
    }
    return value;
}

static void u_1wire_write(ds18b20_u_config_t const* config, uint8_t byte)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        u_bit_write(config->uart, byte & 0x01);
        byte >>= 1;
    }
}

static uint8_t u_1wire_checksum(ds18b20_u_config_t const* config, const uint8_t* data, int len)
{
    int i;
    uint8_t crc = 0;
    for (i = 0; i < len; i++)
    {
        crc = byte_checksum(crc, data[i]);
    }
    return crc;
}

static HAL_StatusTypeDef u_cmd_send(ds18b20_u_config_t const* config, uint8_t cmd)
{
    int i;

    if (u_1wire_reset(config) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (config->rom_code == NULL)
    {
        u_1wire_write(config, DS18B20_ROM_SKIP);
    }
    else
    {
        u_1wire_write(config, DS18B20_ROM_MATCH);
        for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
        u_1wire_write(config, config->rom_code[i]);
    }

    u_1wire_write(config, cmd);

    return HAL_OK;
}

static HAL_StatusTypeDef u_1wire_scratchpad_read(ds18b20_u_config_t const* config, uint8_t* scratchpad)
{
    int i;
    uint8_t crc;
    
    if (u_cmd_send(config, DS18B20_SCRATCHPAD_READ) != HAL_OK)
    {
        return HAL_ERROR;
    }

    for (i = 0; i < DS18B20_SCRATCHPAD_SIZE; i++)
    {
        scratchpad[i] = u_1wire_read(config);
    }

    crc = u_1wire_checksum(config, scratchpad, DS18B20_SCRATCHPAD_SIZE - 1);

    if (scratchpad[DS18B20_SCRATCHPAD_SIZE - 1] == crc)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

static HAL_StatusTypeDef u_1wire_address_read(ds18b20_u_config_t const* config)
{
    int i;
    uint8_t crc;

    if (u_1wire_reset(config) != HAL_OK)
    {
        return HAL_ERROR;
    }

    u_1wire_write(config, DS18B20_ROM_READ);

    for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
    {
        config->rom_code[i] = u_1wire_read(config);
    }

    crc = u_1wire_checksum(config, config->rom_code, DS18B20_ROM_CODE_SIZE - 1);

    if (config->rom_code[DS18B20_ROM_CODE_SIZE - 1] == crc)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}
#endif

#if DS18B20_BITBANG
HAL_StatusTypeDef ds18b20_bb_init(ds18b20_bb_config_t const* config, bool multi_instance)
{

    if (HAL_TIM_Base_Start(config->timer) != HAL_OK)
    {
    	return HAL_ERROR;
    }
    
    if (multi_instance)
    {
    	return bb_1wire_address_read(config);
    }

    return HAL_OK;
}

HAL_StatusTypeDef ds18b20_bb_sampling_start(ds18b20_bb_config_t const* config)
{
  return bb_1wire_cmd_send(config, DS18B20_TEMP_CONVERT);
}

float ds18b20_bb_temp_get(ds18b20_bb_config_t const* config)
{
    uint8_t scratchpad[DS18B20_SCRATCHPAD_SIZE];
    int16_t temp;
    
    if (bb_1wire_scratchpad_read(config, scratchpad) != HAL_OK)
    {
    	return -273.15;
    }
    
    memcpy(&temp, &scratchpad[0], sizeof(temp));
    
    return temp / 16.0f;
}
#endif

#if DS18B20_UART
HAL_StatusTypeDef ds18b20_u_init(ds18b20_u_config_t const* config, bool multi_instance)
{
	if (multi_instance)
	{
		return u_1wire_address_read(config);
	}
    return HAL_OK;
}


HAL_StatusTypeDef ds18b20_u_sampling_start(ds18b20_u_config_t const* config)
{
  return u_cmd_send(config, DS18B20_TEMP_CONVERT);
}

float ds18b20_u_temp_get(ds18b20_u_config_t const* config)
{
    uint8_t scratchpad[DS18B20_SCRATCHPAD_SIZE];
    int16_t temp;
    
    if (u_1wire_scratchpad_read(config, scratchpad) != HAL_OK)
    {
    	return -273.15;
    }
    
    memcpy(&temp, &scratchpad[0], sizeof(temp));
    
    return temp / 16.0f;
}
#endif
