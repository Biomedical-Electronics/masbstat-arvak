#include "main.h"



static uint32_t current=0;
static uint32_t voltage=0;


void initialize_timer(TIM_HandleTypeDef* htim3, double samplingPeriod){
	__HAL_TIM_SET_AUTORELOAD(htim3, (uint32_t)((samplingPeriod*8400.0)));
	__HAL_TIM_SET_COUNTER(htim3, 1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	voltage=ADC_get_val();
	current=ADC_get_val();
}

uint32_t getVoltage(){
	return voltage;
}

uint32_t getCurrent(){
	return current;
}

