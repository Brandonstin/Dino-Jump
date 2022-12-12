/*
 * task_platform.h
 *
 *  Created on: April 20,2022
 *      Author: Brandon Voravongsa, Luca Pizenberg
 */

#ifndef TASK_PLATFORM_H_
#define TASK_PLATFORM_H_

#include <main.h>

#define PLATFORM_QUEUE_LEN  2;

typedef enum {
    NORMAL,
} PLATFORM_TYPE_t;

typedef struct {
    PLATFORM_TYPE_t type;
    uint8_t         id;
    uint8_t         x;
    uint8_t         y;
} PLATFORM_t ;

extern TaskHandle_t Task_Platform_Handle;

/* Variables to store platform information */
extern PLATFORM_t platforms[];
extern uint8_t platformCount;
extern uint8_t columnCount;

/******************************************************************************
* * This function will initialize T32_2 and the random number generator
******************************************************************************/
void platform_init(void);

/******************************************************************************
 * This task manages the movement and generation of platforms
 ******************************************************************************/
void Task_Platform(void *pvParameters);

#endif /* TASK_PLATFORM_H_ */
