/*
 * task_player.h
 *
 *  Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 */

#ifndef TASK_PLAYER_H_
#define TASK_PLAYER_H_

#include <main.h>

#define BASE_DELAY  1
#define PLAYER_QUEUE_LEN 3

typedef enum {
    PLAYER_LEFT,
    PLAYER_RIGHT,
    PLAYER_CENTER,
    PLAYER_JUMP,
    PLAYER_COLOR
} PLAYER_CMD_t;

typedef struct {
    PLAYER_CMD_t cmd;
    uint8_t     speed;
} PLAYER_MSG_t;

extern TaskHandle_t Task_Player_Handle;
extern TaskHandle_t Task_Jump_Handle;
extern QueueHandle_t Queue_PLAYER;
extern SemaphoreHandle_t Sem_LCD_Draw;

/******************************************************************************
 * This function will initialize Queue_PLAYER and initialize the LCD
 ******************************************************************************/
void player_init(void);

/******************************************************************************
 * This task manages the movement of PLAYER. The accelerometer moves PLAYER left
 * or right, and the button S2 makes PLAYER jump. This task is also responsible
 * for general game flow.
 ******************************************************************************/
void Task_Player(void *pvParameters);

#endif /* TASK_PLAYER_H_ */
