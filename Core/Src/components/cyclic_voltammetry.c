#include "cyclic_voltammetry.h"
#include "timers.h"
#include "main.h"
#include "masb_comm_s.h"
#include "mcp4725_driver.h"


double vCell;
double vObjective;
double vReal;
double iReal;
uint8_t cycle;
struct Data_S data;

void cyclic_volt(){

	struct CV_Configuration_S cvConfiguration = MASB_COMM_S_getCvConfiguration();

	vObjective = cvConfiguration.eVertex1;

	MCP4725_SetOutputVoltage(cvConfiguration.eBegin);

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_PIN, GPIO_PIN_SET);

	initialize_timer(*htim3, cvConfiguration.eStep/cvConfiguration.scanRate);
	cycle = 1;
	while (1){
		vReal=calculateVrefVoltage(getVoltage());
		iReal=calculateIcellCurrent(getCurrent());

		data.point=cycle;
		data.timeMs=cvConfiguration.scanRate*cycle;
		data.voltage=vReal;
		data.current=iReal;
		MASB_COMM_S_sendData(data);


		if (vReal == vObjective){
			if (vObjective == cvConfiguration.eVertex1){
				vObjective == cvConfiguration.eVertex2;
			} else{
				if (vObjective == cvConfiguration.eVertex2){
					vObjective = cvConfiguration.eBegin;
				} else{
					if (cycle >= cvConfiguration.cycles){
						break;
					} else{
						vObjective == cvConfiguration.eVertex1;
					}
				}
			}
		} else{
			if (vReal+cvConfiguration.eStep > vObjective){
				MCP4725_SetOutputVoltage(cvConfiguration.eBegin);
			} else{
				MCP4725_SetOutputVoltage(vCell-eStep);
			}
		}
		cycle++;
	}
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_PIN, GPIO_PIN_RESET);
}
