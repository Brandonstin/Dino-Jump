/*
 * task_buzzer.h
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#ifndef TASK_BUZZER_H_
#define TASK_BUZZER_H_

#include <main.h>

extern TaskHandle_t Task_Buzzer_Handle;

/******************************************************************************
 * This function will initialize the buzzer on the MKII
 ******************************************************************************/
void buzzer_init(void);

/******************************************************************************
 * This task manages the game over screen and plays the music from HW2.
 ******************************************************************************/
void Task_Buzzer(void *pvParameters);

#endif /* TASK_BUZZER_H_ */
