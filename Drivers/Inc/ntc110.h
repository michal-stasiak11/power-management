/*
 * ntc110.h
 *
 *  Created on: Aug 26, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_NTC110_H_
#define INC_NTC110_H_

#include "adc.h"

#define NTC110_5K 0
#define NTC110_1K 1

float ntc110_temp_get(uint16_t value, uint8_t resolution);

#endif /* INC_NTC110_H_ */
