/*
 * task_player.c
 *
 *  Created on: April 20, 2022
 *      Author: Brandon Voravongsa, Luca Pizenberg
 */

#include <main.h>

TaskHandle_t Task_Player_Handle;
TaskHandle_t Task_Jump_Handle;
QueueHandle_t Queue_PLAYER;
SemaphoreHandle_t Sem_LCD_Draw;

/******************************************************************************
 * This function will initialize Queue_player and initialize the LCD
 ******************************************************************************/
void player_init(void)
{

    // init the queue used to send player commands
    Queue_PLAYER = xQueueCreate(PLAYER_QUEUE_LEN, sizeof(PLAYER_MSG_t));

    // init the LCD
    Crystalfontz128x128_Init();

}

/******************************************************************************
 * This task manages the movement of player. The accelerometer moves player left
 * or right, and the button S2 makes player jump. The button S2 will send
 * a jump command to the queue,
 ******************************************************************************/
void Task_Player(void *pvParameters)
{

    uint8_t playerX = 64;
    uint8_t playerY = LCD_HORIZONTAL_MAX - (playerHeight / 2);
    uint8_t delayMS = BASE_DELAY;
    PLAYER_MSG_t player_msg;
    BaseType_t status;

    // stores game states
    int i;
    int playerColor = LCD_COLOR_WHITE;
    int height = 0;
    bool jump = false;
    bool falling = false;
    bool standing = true;
    bool platHit = false;
    bool gameStart = false;

    while (1)
    {

        // waits until command received from queue
        status = xQueueReceive(Queue_PLAYER, &player_msg, portMAX_DELAY);

        if (player_msg.cmd == PLAYER_LEFT)
        {                                          // LEFT COMMAND

            // detects boundaries
            if (playerX - 1 >= (playerWidth / 2))
            {

                // moves the player to the left
                playerX--;

                // speed is used to store the delay
                delayMS = player_msg.speed;
            }

        }
        else if (player_msg.cmd == PLAYER_RIGHT)
        {                                  // RIGHT COMMAND

            // detects boundaries
            if (playerX + 1 <= (LCD_HORIZONTAL_MAX - (playerWidth / 2)))
            {

                // moves the player to the right
                playerX++;

                // speed is used to store the delay
                delayMS = player_msg.speed;
            }

        }
        else if (player_msg.cmd == PLAYER_CENTER)
        {                                 // CENTER

            // mentioned above but I don't want to lose points, the speed stores the delay
            delayMS = player_msg.speed;

        }
        else if (player_msg.cmd == PLAYER_COLOR)
        { // COLOR (for dark mode)

            // speed field is used this time to change color of the sprite using light sensor
            if (player_msg.speed)
            {

                // if speed is 1, dark mode.
                playerColor = LCD_COLOR_YELLOW;

            }
            else
            {

                // otherwise keep it in light mode.
                playerColor = LCD_COLOR_WHITE;

            }

        }
        else if (player_msg.cmd == PLAYER_JUMP && standing)
        { // JUMP COMMAND

            // Sends jump command every time jump button is pressed.
            jump = true;
            falling = false;
            standing = false;
            height = 0;
            gameStart = true;

        }

        // if actively jumping
        if (jump)
        {

            // once you reach the maximum height
            if (height >= 70)
            {

                // you start to fall, gravity, boom.
                jump = false;
                falling = true;
                height = 0;

            }
            else
            {

                // increases jump height by 1
                height++;

                // checks if we are below the top of the screen
                if (playerY - 1 >= (playerHeight / 2))
                {

                    // moves the player up 1
                    playerY--;

                }
                // otherwise if we hit the top of the screen
                else
                {

                    // you just fall
                    jump = false;
                    falling = true;
                    height = 0;

                }
            }


        }
        // not jumping
        else
        {

            // tracks if we hit any platforms
            platHit = false;

            //checks all platforms
            for (i = 0; i < platformCount; i++)
            {

                //hitbox detection
                if ((playerX + (playerWidth / 4)
                        >= platforms[i].x - (platformWidth / 2))
                        && (playerX - (playerWidth / 4)
                                <= platforms[i].x + (platformWidth / 2))
                        && (playerY + (playerHeight / 2)
                                == platforms[i].y - (platformHeight / 2)))
                {

                    // we're on a platform
                    platHit = true;
                }
            }

            // checks if we are above bottom of the screen
            if (playerY + 1 >= LCD_HORIZONTAL_MAX - (playerHeight / 2))
            {

                falling = false;
                standing = true;

                // if the game has started
                if (gameStart)
                {

                    // resets the game
                    height = 0;
                    jump = false;
                    falling = false;
                    standing = true;
                    platHit = false;
                    gameStart = false;

                    //resumes task buzzer which calls you a loser
                    vTaskResume(Task_Buzzer_Handle);
                }


            }
            // otherwise you are falling and have hit a platform
            else if (falling && platHit)
            {


                falling = false;
                standing = true;


            }
            // otherwise if you are standing and on a platform
            else if (standing && !platHit)
            {

                falling = true;
                standing = false;

            }

            // currently on a platform?
            if (platHit)
            {

                standing = true;
            }

            // after everything, if you are still falling, keep falling
            if (falling)
            {

                playerY++;

            }
        }

            // takes semaphore so we can write to the LCD
            status = xSemaphoreTake(Sem_LCD_Draw, portMAX_DELAY);

            if (jump)
            {
                lcd_draw_image(playerX, playerY, playerWidth, playerHeight,
                               player_bitmap, playerColor,
                               LCD_COLOR_BLACK);
            }
            else if (player_msg.cmd == PLAYER_RIGHT)
            {

                lcd_draw_image(playerX, playerY, playerWidth, playerHeight,
                               player_bitmap, playerColor,
                               LCD_COLOR_BLACK);

            }
            else if (player_msg.cmd == PLAYER_LEFT)
            {

                lcd_draw_image(playerX, playerY, playerWidth, playerHeight,
                               player_bitmap, playerColor,
                               LCD_COLOR_BLACK);
            }
            else
            {
                lcd_draw_image(playerX, playerY, playerWidth, playerHeight,
                        player_bitmap, playerColor,
                        LCD_COLOR_BLACK);
            }



        // releases the semaphore
        xSemaphoreGive(Sem_LCD_Draw);

        vTaskDelay(pdMS_TO_TICKS(delayMS));
    }

}

