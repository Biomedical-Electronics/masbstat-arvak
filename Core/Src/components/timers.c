#include "main.h"
#include "components/adc.h"


static uint32_t current=0;
static uint32_t voltage=0;
static uint8_t measure=0;


void initialize_timer(TIM_HandleTypeDef* htim3, double samplingPeriod){
	// samplingPeriod is the value of the sampling period introduced by the program in ms
	__HAL_TIM_SET_AUTORELOAD(htim3, (uint32_t)((samplingPeriod*10))); // Sets the TIM Autoreload Register value to sampling period value * 10
	// Default frequency is 10 kHz --> 1e4 Hz (1/s)
	// Sampling period is in ms. Therefore, sampling period * 1e-3 is the value in sec
	// Sampling period in sec (*1e-3) multiplied by default frequency (*1e4 1/s) gives that the result must be multiplied by 10 to obtain the counter period
	__HAL_TIM_SET_COUNTER(htim3, 0); // Sets the TIM Counter Register value to 0 (start).
	HAL_TIM_Base_Start_IT(htim3); // Starts the TIM Base generation.
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) { // If the sampling period has elapsed
	voltage=ADC_get_val(); // first read, returns the voltage read by the ADC
	current=ADC_get_val(); // second read, returns the current read by the ADC
	measure=1; // the measure has to be performed
}

uint32_t getVoltage(){
	return voltage; // return the voltage read
}

uint32_t getCurrent(){
	return current; // return the current read
}

// Function that will return 1 if the sampling period has elapsed and 0 otherwise
uint8_t getMeasure(){
	if (measure==1){ // If the measure has to be performed
		measure=0; // change state from 1 to 0
		return 1; // and perform the measure
	}
	return 0; // do not perform the measure
}
