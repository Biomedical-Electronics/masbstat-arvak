/*
 * PMU.c
 *
 *  Created on: May 9, 2022
 *      Author: Ruben Cuervo & Pere Pena
 */
#include "main.h"

void initialize_PMU(){
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET); // we turn on the PMU
	// we wait 500 ms before continuing with the execution of the program to ensure
	// that all the components of the device are correctly powered.
	HAL_Delay(500);
}
