/*
 * task_buzzer.c
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Buzzer_Handle;

/******************************************************************************
 * This function will initialize the buzzer on the MKII
 ******************************************************************************/
void buzzer_init(void) {
    uint16_t ticks_period = 0;
    // Set P2.7 to be a GPIO OUTPUT Pin
     P2->DIR |= BIT7;

     // the TIMERA PWM controller will control the buzzer
     // on the MKII P2.7 <--> TA0.4
     P2->SEL0 |= BIT7;
     P2->SEL1 &= ~BIT7;

     // Turn off TA0
     TIMER_A0->CTL = 0;

     // Set the period of the timer.
     TIMER_A0->CCR[0] = ticks_period - 1;

     // Configure BUZZER Duty Cycle to 50%
     TIMER_A0->CCR[4] = (ticks_period / 2) - 1;

     // Configure TA0.4 for RESET/SET Mode
     TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;

     // Select the master clock as the timer source
     TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;

}

/******************************************************************************
 * This task manages the game over screen and plays the music from HW2.
 ******************************************************************************/
void Task_Buzzer(void *pvParameters) {

    BaseType_t status;

    // Sees if song played already or not.
    bool songPlayed = false;

    /// Suspends task until it is needed.
    vTaskSuspend(Task_Buzzer_Handle);

    while (1) {

        // Suspend tasks that may mess with this one.
        vTaskSuspend(Task_Accelerometer_Handle);
        vTaskSuspend(Task_Player_Handle);
        vTaskSuspend(Task_Platform_Handle);
        vTaskSuspend(Task_Light_Sensor_Handle);

        // Displays game over screen
        lcd_draw_image(
                LCD_HORIZONTAL_MAX / 2,
                LCD_VERTICAL_MAX / 2,
                splashScreenWidth,
                splashScreenHeight,
                gameover_Bitmap,
                LCD_COLOR_WHITE,
                LCD_COLOR_BLACK
        );

        // If the song hasn't played, play it.
        if (!songPlayed) {

            music_play_song();
            songPlayed = true;

        }

        // Waits until button1 is pressed.
        if (BUTTON1_PRESSED) {

            BUTTON1_PRESSED = false;
            songPlayed = false;

            // resets the game
            lcd_draw_image(
                    LCD_HORIZONTAL_MAX / 2,
                    LCD_VERTICAL_MAX / 2,
                    splashScreenWidth,
                    splashScreenHeight,
                    gameover_Bitmap,
                    LCD_COLOR_BLACK,
                    LCD_COLOR_BLACK
            );

            // resumes other tasks
            vTaskResume(Task_Player_Handle);
            vTaskResume(Task_Platform_Handle);
            vTaskResume(Task_Light_Sensor_Handle);
            vTaskResume(Task_Accelerometer_Handle);


            // suspends the buzzer task
            vTaskSuspend(Task_Buzzer_Handle);

        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


