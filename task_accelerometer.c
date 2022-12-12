/*
 * task_accelerometer.h
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Accelerometer_Handle;
TaskHandle_t Task_Accelerometer_Timer_Handle;

// Holds the X value the accelerometer provides.
volatile uint32_t ACCELEROMETER_X_DIR = 0;

/******************************************************************************
* This function initializes the accelerometer on the MKII
******************************************************************************/
void accel_init(void) {

    // configure the X direction as an analog input P6.1 (A14)
    P6->SEL0 |= BIT1;
    P6->SEL1 |= BIT1;

    // configure CTL0 to sample 16-times in pulsed sample mode.
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP;

    // configure CTL1 to return 14-bit values
    ADC14->CTL1 = ADC14_CTL1_RES_3;

    // associate the ACC_XOUT signal with MEM[0]
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14;

    // enable interrupts after a value has been written into MEM[0]
    ADC14->IER0 |= ADC14_IER0_IE0;

    // enable ADC interrupt
    NVIC_EnableIRQ(ADC14_IRQn);

    NVIC_SetPriority(ADC14_IRQn, 2);

    // Turn ADC on
    ADC14->CTL0 |= ADC14_CTL0_ON;

}

/******************************************************************************
* Used to start an ADC14 conversion
******************************************************************************/
void Task_Accelerometer_Timer(void *pvParameters) {

    while (1) {

        // start an ADC conversion
        ADC14->CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;

        // delay 5ms
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/******************************************************************************
* Bottom Half | Examines the ADC data from the accelerometer
******************************************************************************/
void Task_Accelerometer_Bottom_Half(void *pvParameters) {

    uint32_t ulEventToProcess;
    PLAYER_MSG_t player_msg;
    BaseType_t status;

    while (1) {

        // wait until we get a task notification from the ADC14 ISR
        ulEventToProcess = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ACCELEROMETER_X_DIR > VOLT_1P7) { // tilting right

            // Activates the Task_Player task
            player_msg.cmd = PLAYER_RIGHT;
            player_msg.speed = 1;
            status = xQueueSendToBack(Queue_PLAYER, &player_msg, portMAX_DELAY);

        } else if (ACCELEROMETER_X_DIR < VOLT_1P6) { // tilting left

            player_msg.cmd = PLAYER_LEFT;
            player_msg.speed = 1;
            status = xQueueSendToBack(Queue_PLAYER, &player_msg, portMAX_DELAY);


    } else{ // not tilting

            player_msg.cmd = PLAYER_CENTER;
            player_msg.speed = 1;
            status = xQueueSendToBack(Queue_PLAYER, &player_msg, portMAX_DELAY);

        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/******************************************************************************
* Bottom Half | ADC14 IRQHandler
******************************************************************************/
void ADC14_IRQHandler(void) {

    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    // read the value and clear the interrupt
    ACCELEROMETER_X_DIR = ADC14->MEM[0];

    // send a task notification to Task_Accelerometer_Bottom_Half
    vTaskNotifyGiveFromISR(
            Task_Accelerometer_Handle,
            &xHigherPriorityTaskWoken
    );

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

