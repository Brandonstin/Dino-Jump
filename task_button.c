/*
 * task_button.c
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Button_Handle;

/* Global variables to alert any tasks if button 1 or 2 has been pressed */
volatile extern bool BUTTON1_PRESSED = false;
volatile extern bool BUTTON2_PRESSED = false;

/******************************************************************************
 * This function will initialize the buttons and set T32_1 to interrupt every
 * 10ms
 ******************************************************************************/
void button_init() {

    // Initialize S1 and S2
    P5->DIR &= ~BIT1;
    P3->DIR &= ~BIT5;

    // Have T32_1 generate interupts every 10ms
    // ticks = desired period / core clock period
        uint32_t ticks = ((SystemCoreClock * 10)/1000) - 1;

        // stop the timer
        TIMER32_1->CONTROL = 0;

        // Set the load register
        TIMER32_1->LOAD = ticks;

        // Enable the Timer32 interrupt in NVIC
        // __enable_irq();
        NVIC_EnableIRQ(T32_INT1_IRQn);
        NVIC_SetPriority(T32_INT1_IRQn, 2);

        // Start Timer32 and enable interrupt
        TIMER32_1->CONTROL =    TIMER32_CONTROL_ENABLE |    // turn timer on
                                TIMER32_CONTROL_MODE |      // periodic mode
                                TIMER32_CONTROL_SIZE |      // 32 bit timer
                                TIMER32_CONTROL_IE;         // enable interrupts

}

/******************************************************************************
 * Bottom half of the button task. When it receives an interrupt from the ISR,
 * it will send a jump command to Task_Player.
 ******************************************************************************/
void Task_Button_Bottom_Half(void *pvParameters) {

    uint32_t ulEventToProcess;
    PLAYER_MSG_t player_msg;
    BaseType_t status;

    while (1) {

        /* Wait until we get a task notification from the T32_INT1 ISR */
        ulEventToProcess = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Send a jump command to the queue */
        player_msg.cmd = PLAYER_JUMP;
        player_msg.speed = 1;
        status = xQueueSendToBack(Queue_PLAYER, &player_msg, portMAX_DELAY);

        BUTTON2_PRESSED = false;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/******************************************************************************
 * IRQ Handler for T32_1. Sends a task notification to Task_Button_Bottom_Half
 * if S2 has been de-bounced and pressed. If S1 has been de-bounced it sets the
 * global variable
 ******************************************************************************/
void T32_INT1_IRQHandler() {

    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    if (!BUTTON2_PRESSED) {

        static uint8_t button2_state = 0x00;
        bool button2_pressed = ((P3->IN & BIT5) == 0);

        button2_state = button2_state << 1;

        if(button2_pressed) {

            button2_state |= 0x01;
        }

        // if held for 70ms
        if(button2_state == 0x7F) {

            BUTTON2_PRESSED = true;

            // sends task notification to bottom half button task
            vTaskNotifyGiveFromISR(
                    Task_Button_Handle,
                    &xHigherPriorityTaskWoken
            );

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        }
    }


    if (!BUTTON1_PRESSED) {

        static uint8_t button1_state = 0x00;
        bool button1_pressed = ((P5->IN & BIT1) == 0);

        button1_state = button1_state << 1;

        if(button1_pressed) {

            button1_state |= 0x01;
        }

        if(button1_state == 0x7F) {

            BUTTON1_PRESSED = true;

        }
    }

    /* DONT FORGET TO CLEAR THE INTERRUPT */
    TIMER32_1->INTCLR = BIT0;

}

