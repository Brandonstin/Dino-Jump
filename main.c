/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * main.c
 *
 *  Created on: April 20, 2022
 *      Author: Brandon Voravongsa, Luca Pizenberg
 */

#include "main.h"

/*
 *  ======== main ========
 */
int main(void) {

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // Initializes all peripherals needed
    player_init();
    accel_init();
    button_init();
    platform_init();
    light_init();
    buzzer_init();

    __enable_irq();

    // Creates a binary LCD semaphore
    Sem_LCD_Draw = xSemaphoreCreateBinary();

    // Releases the LCD semaphore
    xSemaphoreGive(Sem_LCD_Draw);

    // Makes the tasks
    xTaskCreate
        (   Task_Accelerometer_Timer,
            "Task_Accelerometer",
            configMINIMAL_STACK_SIZE,
            NULL,
            3,
            &Task_Accelerometer_Timer_Handle
        );

    xTaskCreate
         (   Task_Accelerometer_Bottom_Half,
             "Task_Accelerometer",
             configMINIMAL_STACK_SIZE,
             NULL,
             4,
             &Task_Accelerometer_Handle
         );

    xTaskCreate
         (   Task_Player,
             "Task_Player",
             configMINIMAL_STACK_SIZE,
             NULL,
             2,
             &Task_Player_Handle
         );

    xTaskCreate
         (   Task_Platform,
             "Task_Platform",
             configMINIMAL_STACK_SIZE,
             NULL,
             1,
             &Task_Platform_Handle
         );

    xTaskCreate
         (   Task_Button_Bottom_Half,
             "Task_Button",
             configMINIMAL_STACK_SIZE,
             NULL,
             4,
             &Task_Button_Handle
         );

    xTaskCreate
         (   Task_Buzzer,
             "Task_Buzzer",
             configMINIMAL_STACK_SIZE,
             NULL,
             5,
             &Task_Buzzer_Handle
         );

    xTaskCreate
         (   Task_Light_Sensor_Bottom_Half,
             "Task_LightSensor",
             configMINIMAL_STACK_SIZE,
             NULL,
             4,
             &Task_Light_Sensor_Handle
         );

    xTaskCreate
        (   Task_Splash,
            "Task_Splash",
            configMINIMAL_STACK_SIZE,
            NULL,
            5,
            &Task_Splash_Handle
        );

    // Starts the freeRTOS scheduler
    vTaskStartScheduler();

    while(1){};
    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
