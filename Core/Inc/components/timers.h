/*
 * timers.h
 *
 *  Created on: 10 may. 2022
 *      Authors: Ruben Cuervo & Pere Pena
 */

#ifndef INC_COMPONENTS_TIMERS_H_
#define INC_COMPONENTS_TIMERS_H_

#include "main.h"

void initialize_timer(TIM_HandleTypeDef* htim3, double samplingPeriod);
uint32_t getVoltage();
uint32_t getCurrent();
uint8_t getMeasure();

#endif /* INC_COMPONENTS_TIMERS_H_ */
