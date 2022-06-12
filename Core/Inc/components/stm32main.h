/*
 * stm32main.h
 *
 *  Created on: 10 may. 2022
 *      Authors: Ruben Cuervo & Pere Pena
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "main.h"


struct Handles_S {
    UART_HandleTypeDef *huart;
};

void setup(struct Handles_S *handles);
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
