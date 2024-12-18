#include "gl5516.h"
#include <math.h>

float gl5516_brightness_get(uint16_t value, uint8_t resolution)
{
    /** Coefficients for Steinhart-Hart equation */
    float a = -1.35e-2;
    float b = 2.42e-3;
    float c = -7.87e-6;

    int R = 4700 * ((float)(1 << resolution) / value - 1.0 );

    float logR = log(R);

    float result = 1.0 / ( a + b * logR + c * logR * logR * logR) - 273.15;

    /* Provided kOhm/Lux curve in datasheet does not cover 
    *  brightness under 1 Lux. Thus in a dark room or at night, 
    *  the function will return negative values.
    */
    return (result > 0.0 ? result : 0.0);
}
