/*
 * cyclic_voltammetry.h
 *
 *  Created on: 10 may. 2022
 *      Authors: Ruben Cuervo & Pere Pena
 */

#ifndef INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_
#define INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_

#include "main.h"

struct CV_Configuration_S {
	double eBegin;
	double eVertex1;
	double eVertex2;
	uint8_t cycles;
	double scanRate;
	double eStep;
};

void cyclic_volt();

int compareFloating(double,double);

#endif /* INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_ */
