/*
 * stm32main.c
 *
 *  Created on: May 9, 2022
 *      Author: Ruben Cuervo & Pere Pena
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/PMU.h"
#include "components/chronoamperometry.h"
#include "components/cyclic_voltammetry.h"
#include "components/i2c_lib.h"
#include "components/ad5280_driver.h"
#include "main.h"
extern I2C_HandleTypeDef hi2c1;

uint8_t state;
AD5280_Handle_T hpot = NULL;

MCP4725_Handle_T hdac;
void setup(struct Handles_S *handles) {

	initialize_PMU(); // We initialize the PMU and wait 500 ms (PMU.c)


	I2C_init(&hi2c1); //STM32 only


	hpot = AD5280_Init();

	// We configure its slave I2C address, its total resistance (there are
	// different models; this one has 50kohms) and we indicate what function we
	// want it to write via I2C. We will use the function
	// I2C_Write from the i2c_lib library.
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_write);

	// We fix the resistance to 50 kohms.
	AD5280_SetWBResistance(hpot, 50e3f);
	hdac = MCP4725_Init();
	MCP4725_ConfigSlaveAddress(hdac, 0x66); // SLAVE adress
	MCP4725_ConfigVoltageReference(hdac, 4.0f); // reference tension
	MCP4725_ConfigWriteFunction(hdac, I2C_write); // writing function (I2C_lib)
	MASB_COMM_S_waitForMessage();
}

void loop(void) { //check continuously if a instruction is being send
	if (MASB_COMM_S_dataReceived()) { // if the instruction has been received
		switch(MASB_COMM_S_command()) { // switch to command mode
			case START_CV_MEAS: // if a cyclic voltammetry wants to be started
				state=CV; // change the state to cyclic voltammetry
				break;
			case START_CA_MEAS: // if a chronoamperometry wants to be started
				state=CA; // change the state to chronoamperometry
				break;
			case STOP_MEAS: // if the measure wants to be stopped
				state=IDLE; // change the state to IDLE
				break;
			default:
				break;
		}
		switch(state){ // switch to state mode
			case CV: // if the state is cyclic voltammetry
				cyclic_volt(); // perform the entire cyclic voltammetry sending all the data
				state=IDLE; // stop it (IDLE state)
				break;
			case CA: // if the state is chronoamperometry
				chronoamp(); // perform the entire chronoamperometry sending all the data
				state=IDLE; // stop it (IDLE state)
				break;
			default:
				break;
		}
		MASB_COMM_S_waitForMessage(); // if the instruction has not been received, wait for it
	}

}
