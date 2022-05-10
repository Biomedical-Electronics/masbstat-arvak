#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"

MCP4725_Handle_T hdac;
void setup(struct Handles_S *handles) {
	hdac = MCP4725_Init();
}

void loop(void) {

}
