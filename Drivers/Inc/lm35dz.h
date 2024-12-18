/*
 * lm35dz.h
 *
 *  Created on: Aug 26, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_LM35DZ_H_
#define INC_LM35DZ_H_

#include "adc.h"

float lm35dz_temp_get(uint16_t value, uint8_t resolution);

#endif /* INC_LM35DZ_H_ */