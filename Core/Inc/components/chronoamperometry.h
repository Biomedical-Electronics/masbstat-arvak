/*
 * chronoamperometry.h
 *
 *  Created on: 10 may. 2022
 *      Authors: Ruben Cuervo & Pere Pena
 */

#ifndef INC_COMPONENTS_CHRONOAMPEROMETRY_H_
#define INC_COMPONENTS_CHRONOAMPEROMETRY_H_

#include "stdint.h"

struct CA_Configuration_S {
	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

void chronoamp();

#endif /* INC_COMPONENTS_CHRONOAMPEROMETRY_H_ */
