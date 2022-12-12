/*
 * task_platform.c
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Platform_Handle;

/* Variables to store platform information */
PLATFORM_t platforms[10];
uint8_t platformCount = 6;
uint8_t columnCount = 3;

/* Used for the random number generator seed */
time_t t;

/******************************************************************************
* * This function will initialize T32_2 and the random number generator
******************************************************************************/
void platform_init(void) {

    /* Configure the T32_2 to send an interrupt every 25ms */
    // ticks = desired period / core clock period
        uint32_t ticks = ((SystemCoreClock * 25)/1000) - 1;

        // stop the timer
        TIMER32_2->CONTROL = 0;

        // Set the load register
        TIMER32_2->LOAD = ticks;

        // Enable the Timer32 interrupt in NVIC
        // __enable_irq();
        NVIC_EnableIRQ(T32_INT2_IRQn);
        NVIC_SetPriority(T32_INT2_IRQn, 1);

        // Start Timer32 and enable interrupt
        TIMER32_2->CONTROL =    TIMER32_CONTROL_ENABLE |    // turn timer on
                                TIMER32_CONTROL_MODE |      // periodic mode
                                TIMER32_CONTROL_SIZE |      // 32 bit timer
                                TIMER32_CONTROL_IE;         // enable interrupts

    /* Initialize the random number generator */
    srand((unsigned) &(t));

}

/******************************************************************************
 * This task manages the movement and generation of platforms
 ******************************************************************************/
void Task_Platform(void *pvParameters) {

    uint32_t ulEventToProcess;
    BaseType_t status;

    int i;

    // makes the first platforms
    for (i = 0; i < platformCount; i++) {

        platforms[i].type = NORMAL;
        platforms[i].id = i;
        platforms[i].x = ((LCD_HORIZONTAL_MAX - (((i % columnCount) + 1) * (LCD_HORIZONTAL_MAX / (columnCount + 1)))));
        platforms[i].y = (LCD_VERTICAL_MAX - ((i  +  1) * (255 / platformCount)));

    }

    while(1) {

        // wait to receive task noti from t32_1
        ulEventToProcess = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Takes the LCD semaphore
        status = xSemaphoreTake(Sem_LCD_Draw, portMAX_DELAY);

        // for each platform
        for (i = 0; i < platformCount; i++) {

            // When platform is almost offscreen
            if ((platforms[i].y + 1) == (LCD_VERTICAL_MAX - (platformHeight / 2))) {

                // draws black to hid unwanted LCD
                lcd_draw_image(
                        platforms[i].x,
                        platforms[i].y,
                        platformWidth,
                        platformHeight,
                        platform_bitmap,
                        LCD_COLOR_BLACK,
                        LCD_COLOR_BLACK
                );

                // sets y of platform to be just offscreen
                platforms[i].y += platformHeight;

                // gives random x value to platforms
                platforms[i].x = ((LCD_HORIZONTAL_MAX - (((rand() % columnCount) + 1) * (LCD_HORIZONTAL_MAX / (columnCount + 1)))));

            }

            // updates platforms location
            lcd_draw_image(
                    platforms[i].x,
                    platforms[i].y++,
                    platformWidth,
                    platformHeight,
                    platform_bitmap,
                    LCD_COLOR_YELLOW,
                    LCD_COLOR_BLACK
            );
        }

        /* Return the semaphore */
        xSemaphoreGive(Sem_LCD_Draw);

        vTaskDelay(pdMS_TO_TICKS(10));

    }
}

/******************************************************************************
 * IRQ Handler for T32_2. Sends a task notification to Task_Light_Sensor
 * every 200ms. Otherwise send a task notification to Task_Platform
 ******************************************************************************/
void T32_INT2_IRQHandler() {

    // Sometimes calls light sensor task
    static uint8_t read_light = 0x00;
    read_light = read_light << 1;
    read_light |= 0x01;

    if (read_light == 0xFF) {

        BaseType_t xHigherPriorityTaskWoken = pdTRUE;

        vTaskNotifyGiveFromISR(
                Task_Light_Sensor_Handle,
                &xHigherPriorityTaskWoken
        );

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        read_light = 0x00;

    // Mostly call platform task
    } else {

        BaseType_t xHigherPriorityTaskWoken = pdTRUE;

        vTaskNotifyGiveFromISR(
                Task_Platform_Handle,
                &xHigherPriorityTaskWoken
        );

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    }

    // clears interrupt
    TIMER32_2->INTCLR = BIT0;

}

