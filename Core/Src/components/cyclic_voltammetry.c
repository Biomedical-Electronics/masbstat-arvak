/*
 * cyclic_voltammetry.c
 *
 *  Created on: 10 may. 2022
 *      Author: Ruben Cuervo & Pere Pena
 */
#include "components/cyclic_voltammetry.h"
#include "components/timers.h"
#include "main.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/formulas.h"


double vObjective;
double vReal;
double iReal;
extern MCP4725_Handle_T hdac;
extern TIM_HandleTypeDef htim3;
extern ADC_HandleTypeDef hadc1;
uint8_t cycle;
uint8_t end;
uint64_t point;
struct Data_S data;
double eStep;
double vdesired;
int sign;

void cyclic_volt(){

	struct CV_Configuration_S cvConfiguration = MASB_COMM_S_getCvConfiguration(); // Obtain the cyclic voltammetry configuration

	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(cvConfiguration.eBegin)); // Fix Vcell to eBegin

	vObjective = cvConfiguration.eVertex1; // Set the objective voltage to vertex 1

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // Close the relay

	HAL_Delay(10); // Addition of a small delay so as the circuit stabilizes

	double sampling_period=cvConfiguration.eStep/(cvConfiguration.scanRate/1000.0); // Commute the sampling period (decimal optimization)

	initialize_timer(&htim3, sampling_period); // Timer initialization with the known sampling period

	// Initialization of the variables for the loop
	cycle = 1;
	end=0;
	point=0;
	vdesired=cvConfiguration.eBegin; // Voltage we want to arrive at short term
	eStep=cvConfiguration.eStep;

	if (vObjective>cvConfiguration.eVertex2){ // If the objective voltage is bigger than vertex 2
		sign=1; // The sign will be positive
	} else{ // If the objective voltage is smaller than vertex 2
		sign=-1; // The sign will be negative (start the movement to the other side)
	}

	while (1){
		if (getMeasure()){ // If the sampling period has elapsed

			vReal=calculateVrefVoltage(getVoltage()); // Obtain the Vreal voltage by using the formula found in formulas.c
			iReal=calculateIcellCurrent(getCurrent()); // Obtain the current by using the formula found in formulas.c

			data.point=point;
			data.timeMs=(uint32_t)(sampling_period*(double)point); // Increase the time
			data.voltage=vReal;
			data.current=iReal;
			MASB_COMM_S_sendData(data); // Send data to host

			while (1){
				if (compareFloating(vdesired,vObjective)){ // If the voltage we want to arrive at short term is equal to the objective
					if (compareFloating(vObjective,cvConfiguration.eVertex1)){ // If the objective is equal to vertex 1 (border)
						vObjective = cvConfiguration.eVertex2; // Change the objective to vertex 2 (opposite border)
						sign=-sign; // Change the sign to move to the opposite direction
					} else{ // If the objective is not equal to the vertex 1
						if (compareFloating(vObjective,cvConfiguration.eVertex2)){ // If the objective is equal to vertex 2 (border)
							vObjective = cvConfiguration.eBegin; // Change the objective to the starting point
							sign=-sign; // Change the sign to move to the opposite direction
						} else{ // If the objective is also not equal to vertex 2
							if (cycle >= cvConfiguration.cycles){ // we check if it is the last cycle
								end=1; // Stop the measurement
								break;
							} else{ // if it is not the last cycle
								cycle++; // Increase the number of cycle
								vObjective = cvConfiguration.eVertex1; // Set again the objective to vertex 1
							}
						}
					}
				} else{ // If the voltage we want to arrive at short term is NOT equal to the objective
					if ((vdesired+sign*eStep)*sign-vObjective*sign>0){ // If we surpass the objective voltage
						MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(vObjective)); // Fix Vcell to the objective voltage
						vdesired=vObjective; // The voltage we want to arrive at short term is now the objective voltage
					} else{ // If we do not surpass the objective voltage
						vdesired=vdesired+sign*eStep; // Apply the increment
						MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(vdesired)); // Fix Vcell to the voltage we want to arrive at short term
					}
					break;
				}
			}
			point++;
			if (end==1){ // Stop the measurement
				break;
			}
		}
	}
	HAL_TIM_Base_Stop_IT(&htim3); // Stop the timer
	HAL_ADC_Stop(&hadc1); // Stop the ADC
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); // Open the ADC
}

// Function to compare two long floating point numbers
// If we did not use that, the two values would never be equal
int compareFloating(double x, double y){
	return (x-y)*(x-y)<0.00001;
}
