#include "adc.h"

void adc_values_poll(adc_config_t const* config)
{
    HAL_ADC_Start(config->adc);
    for(uint8_t i = 0; i < config->channels; i++)
    {
        HAL_ADC_PollForConversion(config->adc, HAL_MAX_DELAY);
        config->buffer[i] = HAL_ADC_GetValue(config->adc);
    }
}

float adc_sensor_value_get(adc_sensor_config_t const* config)
{
    uint16_t value = config->adc_config->buffer[config->channel];
    return config->converter(value, config->adc_config->resolution);
}
