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

	struct CV_Configuration_S cvConfiguration = MASB_COMM_S_getCvConfiguration();

	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(cvConfiguration.eBegin));

	vObjective = cvConfiguration.eVertex1;

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

	HAL_Delay(10); // Adding a little delay

	initialize_timer(&htim3, cvConfiguration.eStep/cvConfiguration.scanRate*1000);
	cycle = 1;
	end=0;
	point=0;
	vdesired=cvConfiguration.eBegin;
	eStep=cvConfiguration.eStep;
	if (vObjective>cvConfiguration.eVertex2){
		sign=1;
	} else{
		sign=-1;
	}

	while (1){
		if (getMeasure()){
			vReal=calculateVrefVoltage(getVoltage());
			iReal=calculateIcellCurrent(getCurrent());

			data.point=point;
			data.timeMs=cvConfiguration.eStep/cvConfiguration.scanRate*1000*point;
			data.voltage=vReal;
			data.current=iReal;
			MASB_COMM_S_sendData(data);

			while (1){
				if (compareFloating(vdesired,vObjective)){
					if (compareFloating(vObjective,cvConfiguration.eVertex1)){
						vObjective = cvConfiguration.eVertex2;
						sign=-sign;
					} else{
						if (compareFloating(vObjective,cvConfiguration.eVertex2)){
							vObjective = cvConfiguration.eBegin;
							sign=-sign;
						} else{
							if (cycle >= cvConfiguration.cycles){
								end=1;
								break;
							} else{
								cycle++;
								vObjective = cvConfiguration.eVertex1;
							}
						}
					}
				} else{
					if ((vdesired+sign*eStep)*sign-vObjective*sign>0){
						MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(vObjective));
						vdesired=vObjective;
					} else{
						vdesired=vdesired+sign*eStep;
						MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(vdesired));
					}
					break;
				}
			}
			point++;
			if (end==1){
				break;
			}
		}
	}
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

// Function to compare two long floating point numbers
int compareFloating(double x, double y){
	return (x-y)*(x-y)<0.00001;
}
