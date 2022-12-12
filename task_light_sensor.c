/*
 * task_light_sensor.c
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Light_Sensor_Handle;

/******************************************************************************
* This function initializes the functionality of the I2C bus
******************************************************************************/
void light_init(void) {

    // Initialize i2c
    i2c_init();

    // initialize the light sensor
    i2c_write_16(I2C_LIGHT_ADDR, I2C_LIGHT_CONFIG, OPT3001_RST | OPT3001_CVN800MS | OPT3001_MD_CONT);

}

/******************************************************************************
* Examines the raw data read from the light sensor and sends a message to
* Task_Player
******************************************************************************/
void Task_Light_Sensor_Bottom_Half(void *pvParameters) {

    BaseType_t ulEventToProcess;
    BaseType_t status;
    PLAYER_MSG_t player_msg;
    uint16_t light_val;
    uint16_t lux;

    while (1) {

        // waits until task notification is received from T32 IRQ
        ulEventToProcess = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // reads the ilght value
        light_val = OPT3001_read_light();

        // uses lux conversion equation
        lux = (.01 * pow(2, (light_val & 0xF000)) * (light_val & 0x0FFF));


        if (lux < 100) {

            // changes color to yellow
            player_msg.cmd = PLAYER_COLOR;
            player_msg.speed = 1;
            status = xQueueSendToBack(Queue_PLAYER, &player_msg, portMAX_DELAY);

        } else {

            // changes color to white
            player_msg.cmd = PLAYER_COLOR;
            player_msg.speed = 0;
            status = xQueueSendToBack(Queue_PLAYER, &player_msg, portMAX_DELAY);

        }

        vTaskDelay(pdMS_TO_TICKS(10));

    }
}

/******************************************************************************
* Reads and returns the raw data from the light sensor
******************************************************************************/
uint16_t OPT3001_read_light(void) {

    uint16_t light_val = 0;

    // reads value in result reg in the light sensor
    light_val = i2c_read_16(I2C_LIGHT_ADDR, I2C_LIGHT_RESULT);

    return light_val;

}



