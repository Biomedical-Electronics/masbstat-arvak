#include "main.h"
#include "components/adc.h"


static uint32_t current=0;
static uint32_t voltage=0;
static uint8_t measure=0;


void initialize_timer(TIM_HandleTypeDef* htim3, double samplingPeriod){
	__HAL_TIM_SET_AUTORELOAD(htim3, (uint32_t)((samplingPeriod*10)));
	__HAL_TIM_SET_COUNTER(htim3, 0);
	HAL_TIM_Base_Start_IT(htim3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) {
	voltage=ADC_get_val();
	current=ADC_get_val();
	measure=1;
}

uint32_t getVoltage(){
	return voltage;
}

uint32_t getCurrent(){
	return current;
}

uint8_t getMeasure(){
	if (measure==1){
		measure=0;
		return 1;
	}
	return 0;
}
