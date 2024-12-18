/*
 * syh2r.h
 *
 *  Created on: Aug 26, 2024
 *      Author: Michał Stasiak
 */

#ifndef INC_SYH2R_H_
#define INC_SYH2R_H_

#include "adc.h"

float syh2r_humidity_get(uint16_t value, uint8_t resolution);

#endif /* INC_SYH2R_H_ */