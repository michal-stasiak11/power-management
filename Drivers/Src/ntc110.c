#include "ntc110.h"
#include <math.h>

float ntc110_temp_get(uint16_t value, uint8_t resolution)
{
    /** Coefficients for Steinhart-Hart equation and resistance for resistor in voltage divider*/
#if NTC110_5K
    float a = 4.46e-3;
    float b = -3.34e-4;
    float c = 2.74e-6;

    int R = 4700 * ((float)(1 << resolution) / value - 1.0 );
#endif

#if NTC110_1K
    float a = 2.52e-3;
    float b = 5.46e-5;
    float c = 1.29e-6;

    int R = 1000 * ((float)(1 << resolution) / value - 1.0 );
#endif

    float logR = log(R);

    return 1.0 / ( a + b * logR + c * logR * logR * logR) - 273.15;
}