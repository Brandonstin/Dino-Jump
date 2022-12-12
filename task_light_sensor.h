/*
 * task_light_sensor.h
 *
 *  Created on: April, 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#ifndef TASK_LIGHT_SENSOR_H_
#define TASK_LIGHT_SENSOR_H_

#include <main.h>

#define I2C_LIGHT_ADDR      0x44

#define I2C_LIGHT_RESULT    0x00
#define I2C_LIGHT_CONFIG    0x01
#define I2C_LIGHT_LO_LIM    0x02
#define I2C_LIGHT_HI_LIM    0x03
#define I2C_LIGHT_MAN_ID    0x7E
#define I2C_LIGHT_DEV_ID    0x7F

/* Configuration registers */
#define OPT3001_RST         0xC810
#define OPT3001_FLG_LOW     0x0020
#define OPT3001_FLG_HIGH    0x0040
#define OPT3001_CVN800MS    0x0800
#define OPT3001_MD_CONT     0x0600
#define OPT3001_CNV_RDY     0x0080

extern TaskHandle_t Task_Light_Sensor_Handle;

/******************************************************************************
* This function initializes the functionality of the I2C bus
******************************************************************************/
void light_init(void);

/******************************************************************************
* Examines the raw data read from the light sensor and sends a message to
* Task_Player
******************************************************************************/
void Task_Light_Sensor_Bottom_Half(void *pvParameters);

/******************************************************************************
* Reads and returns the raw data from the light sensor
******************************************************************************/
uint16_t OPT3001_read_light(void);

#endif /* TASK_LIGHT_SENSOR_H_ */
