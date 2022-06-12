/*
 * chronoamperometry.c
 *
 *  Created on: 10 may. 2022
 *      Author: Ruben Cuervo & Pere Pena
 */

#include "components/chronoamperometry.h"
#include "components/timers.h"
#include "main.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/formulas.h"

extern MCP4725_Handle_T hdac;
extern TIM_HandleTypeDef htim3;
extern ADC_HandleTypeDef hadc1;
struct CA_Configuration_S caConfiguration;
double vReal;
double iReal;
uint32_t cycle;
struct Data_S data;

void chronoamp(){

	caConfiguration = MASB_COMM_S_getCaConfiguration(); // Obtain the chronoamperometry configuration

	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(caConfiguration.eDC)); // Fix Vcell to eDC

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // Close the relay

	HAL_Delay(10); // Addition of a small delay so as the circuit stabilizes

	initialize_timer(&htim3, caConfiguration.samplingPeriodMs); // Timer initialization with the known sampling period

	cycle=0;

	while (1){
		if (getMeasure()){ // If the sampling period has elapsed

			vReal=calculateVrefVoltage(getVoltage()); // Obtain the Vreal voltage by using the formula found in formulas.c
			iReal=calculateIcellCurrent(getCurrent()); // Obtain the current by using the formula found in formulas.c

			data.point=cycle+1; // Number of cycles +1 because we start at 2 (not 1)
			data.timeMs=cycle*caConfiguration.samplingPeriodMs; // Increase the time
			data.voltage=vReal;
			data.current=iReal;

			MASB_COMM_S_sendData(data); // Send data to host
			cycle++; // Increasing the number of cycle

			if (data.timeMs>=caConfiguration.measurementTime*1000){ // If measurement time has elapsed
				break; // Stop the measurement
			}
		}
	}
	HAL_TIM_Base_Stop_IT(&htim3); // Stop the timer
	HAL_ADC_Stop(&hadc1); // Stop the ADC
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); // Open the ADC
}
