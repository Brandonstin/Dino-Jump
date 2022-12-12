/*
 * task_button.h
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#ifndef TASK_BUTTON_H_
#define TASK_BUTTON_H_

#include <main.h>

extern TaskHandle_t Task_Button_Handle;

volatile extern bool BUTTON1_PRESSED;
volatile extern bool BUTTON2_PRESSED;

/******************************************************************************
 * This function will initialize the buttons and set T32_1 to interrupt every
 * 10ms
 ******************************************************************************/
void button_init(void);

/******************************************************************************
 * Bottom half of the button task. When it receives an interrupt from the ISR,
 * it will send a jump command to Task_Player.
 ******************************************************************************/
void Task_Button_Bottom_Half(void *pvParameters);

#endif /* TASK_BUTTON_H_ */
