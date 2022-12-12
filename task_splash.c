
/*
 * task_splash.c
 *
 *  Created on: April 22, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Splash_Handle;

void Task_Splash(void *pvParameters) {

    // Pause other game tasks
    vTaskSuspend(Task_Accelerometer_Handle);
    vTaskSuspend(Task_Player_Handle);
    vTaskSuspend(Task_Platform_Handle);

    BaseType_t ulEventToProcess;
    BaseType_t status;

    bool anim = true;
    uint8_t i = 40;

    // draw splash screen
    lcd_draw_image(
            LCD_HORIZONTAL_MAX / 2,
            LCD_VERTICAL_MAX / 2,
            splashScreenWidth,
            splashScreenHeight,
            splashScreen0_Bitmap,
            LCD_COLOR_WHITE,
            LCD_COLOR_BLACK
    );

    // draw player on splash screen
    while (1) {

        lcd_draw_image(
                LCD_HORIZONTAL_MAX / 2,
                LCD_VERTICAL_MAX /2 ,
                playerWidth,
                playerHeight,
                player_bitmap,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK
        );

        // if player "animation" is up
        if (anim) {
            // increment height
            i++;
        } else {
            // else decrement height
            i--;
        }

        // if player at maximum high, disable up "animation"
        if (anim && i >= 80) {
            anim = false;
        } else if (!anim && i <= 70) {
            anim = true;
        }

        // if button S1 pressed
        if (BUTTON1_PRESSED) {

            BUTTON1_PRESSED = false;

            // redraw game splash screen
            lcd_draw_image(
                    LCD_HORIZONTAL_MAX / 2,
                    LCD_VERTICAL_MAX / 2,
                    splashScreenWidth,
                    splashScreenHeight,
                    splashScreen0_Bitmap,
                    LCD_COLOR_BLACK,
                    LCD_COLOR_BLACK
            );

            // resume game tasks
            vTaskResume(Task_Accelerometer_Handle);
            vTaskResume(Task_Player_Handle);
            vTaskResume(Task_Platform_Handle);

            // suspend splash task
            vTaskSuspend(Task_Splash_Handle);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}





