/*
 * chronoamperometry.c
 *
 *  Created on: 10 may. 2022
 *      Author: Rubén Cuervo & Pere Pena
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

	caConfiguration = MASB_COMM_S_getCaConfiguration();

	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(caConfiguration.eDC));

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	HAL_Delay(10); // Ading a little delay

	initialize_timer(&htim3, caConfiguration.samplingPeriodMs);
	while (1){
			if (getMeasure()){
				vReal=calculateVrefVoltage(getVoltage());
				iReal=calculateIcellCurrent(getCurrent());

				data.point=1;
				data.timeMs=0;
				data.voltage=vReal;
				data.current=iReal;

				MASB_COMM_S_sendData(data);
				break;
			}
	}

	cycle=1;

	while (1){
		if (getMeasure()){
			vReal=calculateVrefVoltage(getVoltage());
			iReal=calculateIcellCurrent(getCurrent());

			data.point=cycle+1;
			data.timeMs=cycle*caConfiguration.samplingPeriodMs;
			data.voltage=vReal;
			data.current=iReal;

			MASB_COMM_S_sendData(data);
			cycle++;

			if (data.timeMs>=caConfiguration.measurementTime*1000){
				break;
			}
		}
	}
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}
