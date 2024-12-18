#include "lm35dz.h"

float lm35dz_temp_get(uint16_t value, uint8_t resolution)
{
    float V = 5.0 * value / (1 << resolution);

    return V * 100.0;
}