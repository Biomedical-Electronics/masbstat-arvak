#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/PMU.h"
#include "components/chronoamperometry.h"
#include "components/cyclic_voltammetry.h"

uint8_t state;

MCP4725_Handle_T hdac;
void setup(struct Handles_S *handles) {
	hdac = MCP4725_Init();
	initialize_PMU();
}

void loop(void) {
	if (MASB_COMM_S_dataReceived()) {
		switch(MASB_COMM_S_command()) {
			case START_CV_MEAS:
				state=CV;
				break;
			case START_CA_MEAS:
				state=CA;
				break;
			case STOP_MEAS:
				state=IDLE;
				break;
			default:
				break;
		}
	} else{
		switch(state){
			case CV:
				cyclic_volt();
				state=IDLE;
				break;
			case CA:
				chronoamp();
				state=IDLE;
				break;
			default:
				break;
		}
	}
}
