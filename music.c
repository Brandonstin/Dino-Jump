/*
 * music.c
 * Created on: April 20, 2022
 *  Author: Brandon Voravongsa, Luca Pizenberg
 *
 */

#include "music.h"

Note_t Song[] = {{NOTE_C6,ONE_QUARTER,true},  // Tone, Time, Delay
{NOTE_B5,ONE_QUARTER,true},
{NOTE_D6,ONE_QUARTER,true},
{NOTE_C6,ONE_QUARTER,true},
{NOTE_F6,ONE_QUARTER,true},
{NOTE_E6,ONE_QUARTER,true},
{NOTE_G6,ONE_QUARTER,true},
{NOTE_F6,ONE_QUARTER,true},
{NOTE_A6,ONE_QUARTER,true},
{NOTE_A6,ONE_QUARTER,true},
{NOTE_A6,ONE_QUARTER,true},
{NOTE_A6,ONE_QUARTER,true},
{NOTE_A6,ONE_HALF,false},
{NOTE_A6,ONE_HALF,true},
{NOTE_G6,ONE_QUARTER,true},
{NOTE_F6S,ONE_QUARTER,true},
{NOTE_G6,ONE_QUARTER,true},
{NOTE_A6,ONE_QUARTER,true},
{NOTE_F6,ONE_QUARTER,true},
{NOTE_G6,ONE_QUARTER,true},
{NOTE_A6,ONE_QUARTER,true},
{NOTE_F6,ONE_QUARTER,true},
{NOTE_E6,ONE_QUARTER,true},
{NOTE_D6,ONE_QUARTER,true},
{NOTE_E6,ONE_QUARTER,true},
{NOTE_F6,ONE_QUARTER,true},
{NOTE_G6,ONE_HALF,false},
{NOTE_G6,ONE_HALF,true}};

//***************************************************************
// This function returns how long an individual note is played
//***************************************************************
uint32_t music_get_time_delay(measure_time_t time)
{
    uint32_t time_return = 0;

    time_return = MEASURE_DURATION * MEASURE_RATIO;

    switch (time)
    {
    case ONE_QUARTER:
    {
        time_return = time_return / 4;
        break;
    }
    case ONE_HALF:
    {
        time_return = time_return / 2;
        break;
    }
    case ONE_EIGTH:
    {
        time_return = time_return / 8;
        break;
    }
    case THREE_EIGTH:
    {
        time_return = time_return * 3;
        time_return = time_return / 8;
        break;
    }
    default:
    {
        break;
    }
    }

    return time_return - DELAY_AMOUNT;

}

//***************************************************************************
// Plays a single note of the song based on note_index.  After
// the note is played, there is an optional delay in between
// notes.
//
// Examples
// Song[note_index].period       -- Used to determine the period of the
//                                  PWM pulse.
//
// Song[note_index].time         -- 1/4 or 1/2 time note.  Call
//                                  music_get_time_delay() to determine how
//                                  long to play the note
//
// Song[note_index].delay        -- If true, add a period of silence for
//                                  DELAY_AMOUNT
//
//                                  If false, return without delaying.
//*************************************************************************
static void music_play_note(uint16_t note_index)
{

    uint32_t song_delay;

    // set the pwm pulse
    set_PWM_pulse(Song[note_index].period);

    // get the proper song delay
    song_delay = music_get_time_delay(Song[note_index].time);

    // play the buzzer for the certain delay
    // clear the current Mode Control Bits
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;

    // set Mode Control to UP and Clear the current count
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

    // delay
    // STOP THE TIMER
    uint32_t ticks = song_delay;
    TIMER32_1->CONTROL = 0;

    // Set the timer to be a 32-bit, one-shot
    TIMER32_1->CONTROL = TIMER32_CONTROL_ONESHOT | TIMER32_CONTROL_SIZE;

    // Set the load register
    TIMER32_1->LOAD = ticks;

    // Start the timer
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE;

    // Wait until it reaches 0
    while (TIMER32_1->VALUE != 0)
    {
        // timer is busy waiting
    }

    // turn the buzzer off
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;

    // if post-note delay is true
    if (Song[note_index].delay)
    {


        // STOP THE TIMER
        TIMER32_1->CONTROL = 0;

        // Set the timer to be a 32-bit, one-shot
        TIMER32_1->CONTROL = TIMER32_CONTROL_ONESHOT | TIMER32_CONTROL_SIZE;

        // Set the load register
        TIMER32_1->LOAD = DELAY_AMOUNT;

        // Start the timer
        TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE;

        // Wait until it reaches 0
        while (TIMER32_1->VALUE != 0)
        {
            // timer is busy waiting
        }
    }
}

//***************************************************************
// Plays the song (loop through, playing each note)
// and then returns
//***************************************************************
void music_play_song(void)
{

    int i = 0;

    // loop through and play every note in the song
    for (i = 0; i < SONG_NUM_NOTES + 1; i++)
    {
        music_play_note(i);
    }

    // reinitialize the timer32_1 interrupt
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

//***************************************************************
// Modify the TIMERA registers to reflect the necessary
// pulse needed to produce certain notes
//***************************************************************
void set_PWM_pulse(uint16_t pwm_period)
{

    // Set the period of the timer.
    TIMER_A0->CCR[0] = pwm_period - 1;

    // Configure BUZZER Duty Cycle to 50%
    TIMER_A0->CCR[4] = (pwm_period / 2) - 1;

}
