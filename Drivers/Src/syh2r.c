#include "syh2r.h"
#include <math.h>

float syh2r_humidity_get(uint16_t value, uint8_t resolution)
{
    /** Coefficients for Steinhart-Hart equation */
    float a = 2.05e-3;
    float b = 9.35e-5;
    float c = -2.29e-8;

    int R = 10000 * ((float)(1 << resolution) / value - 1.0 );

    float logR = log(R);

    return 1.0 / ( a + b * logR + c * logR * logR * logR) - 273.15;
}
