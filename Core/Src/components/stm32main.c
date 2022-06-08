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

	initialize_PMU();
	I2C_init(&hi2c1); //STM32 only



	hpot = AD5280_Init();

	// Configuramos su direccion I2C de esclavo, su resistencia total (hay
	// diferentes modelos; este tiene 50kohms) e indicamos que funcion queremos que
	// se encargue de la escritura a traves del I2C. Utilizaremos la funcion
	// I2C_Write de la libreria i2c_lib.
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_write);

	// Fijamos la resistencia de 50 kohms.
	AD5280_SetWBResistance(hpot, 50e3f);
	hdac = MCP4725_Init();
	MCP4725_ConfigSlaveAddress(hdac, 0x66); // DIRECCION DEL ESCLAVO
	MCP4725_ConfigVoltageReference(hdac, 4.0f); // TENSION DE REFERENCIA
	MCP4725_ConfigWriteFunction(hdac, I2C_write); // FUNCION DE ESCRITURA (libreria I2C_lib)
	MASB_COMM_S_waitForMessage();
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
		MASB_COMM_S_waitForMessage();
	}

}
