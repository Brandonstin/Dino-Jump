/*
 * task_accelerometer.h
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#ifndef TASK_ACCELEROMETER_H_
#define TASK_ACCELEROMETER_H_

#include "main.h"

// tilt left threshold
#define VOLT_1P6    7943
#define VOLT_1P55   7695
#define VOLT_1P5    7447
#define VOLT_1P45   7199
// tilt right threshold
#define VOLT_1P7    8440
#define VOLT_1P75   8688
#define VOLT_1P8    8936
#define VOLT_1P85   9184

extern TaskHandle_t Task_Accelerometer_Handle;
extern TaskHandle_t Task_Accelerometer_Timer_Handle;

/******************************************************************************
* This function initializes the accelerometer on the MKII
******************************************************************************/
void accel_init(void);

/******************************************************************************
* Used to start an ADC14 Conversion
******************************************************************************/
void Task_Accelerometer_Timer(void *pvParameters);

/******************************************************************************
* Examines the ADC data from the accelerometer
******************************************************************************/
void Task_Accelerometer_Bottom_Half(void *pvParameters);

#endif /* TASK_ACCELEROMETER_H_ */
