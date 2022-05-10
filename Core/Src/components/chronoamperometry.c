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


struct CA_Configuration_S caConfiguration;
double vReal;
double iReal;
uint8_t cycle;
struct Data_S data;

void cyclic_volt(){

	caConfiguration = MASB_COMM_S_getCaConfiguration();

	MCP4725_SetOutputVoltage(caConfiguration.eDC);

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_PIN, GPIO_PIN_SET);

	initialize_timer(*htim3, caConfiguration.samplingPeriodMs);
	cycle = 1;
	while (1){
		vReal=calculateVrefVoltage(getVoltage());
		iReal=calculateIcellCurrent(getCurrent());

		data.point=cycle;
		data.timeMs=cycle*caConfiguration.samplingPeriodMs;
		data.voltage=vReal;
		data.current=iReal;

		MASB_COMM_S_sendData(data);
		cycle++;

		if (data.timeMs>=caConfiguration.measurementTime){
			break;
		}
	}
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_PIN, GPIO_PIN_RESET);
}
