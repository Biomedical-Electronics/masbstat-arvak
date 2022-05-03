/*
 * adc.c
 *
 *  Created on: 3 may. 2022
 *      Author: Rubén Cuervo & Pere Pena
 */

extern ADC_HandleTypeDef hadc1;

// Function that reads the ADC values and returns them in uint32_t format
uint32_t ADC_get_val() {
	HAL_ADC_Start(&hadc1); // We start the conversion
	HAL_ADC_PollForConversion(&hadc1, 300); // Poll for regular conversion complete. (300 ms)
	potADC = HAL_ADC_GetValue(&hadc1);  // We read the result of the conversion and we save it in potADC
	return potADC;
}
