/*
 * music.h
 *  Created on: April 20, 2022
 *      Author: Brandon Voravongsa, Luca Pizenberg
 *
 */

#ifndef MUSIC_H_
#define MUSIC_H_

#include <stdint.h>
#include <stdbool.h>
#include "msp.h"


//*****************************************************************************
// You will need to determine the number of SMCLKs it will
// take for the following notes and thier associated frequencies.
//
// Assume that your MCU is running at 24MHz.
//*****************************************************************************
#define NOTE_A5        27273 // 880 Hz
#define NOTE_A5S       25751 // 932 Hz
#define NOTE_B5        24291 // 988 Hz
#define NOTE_C6        22945 // 1046 Hz
#define NOTE_C6S       21641 // 1109 Hz
#define NOTE_D6        20426 // 1175 Hz
#define NOTE_D6S       19277 // 1245 Hz
#define NOTE_E6        18196 // 1319 Hz
#define NOTE_F6        17180 // 1397 Hz
#define NOTE_F6S       16216 // 1480 Hz
#define NOTE_G6        15306 // 1568 Hz
#define NOTE_G6S       14449 // 1661 Hz
#define NOTE_A6        13636 // 1760 Hz
#define NOTE_A6S       12869 // 1865 Hz
#define NOTE_B6        12146 // 1976 Hz
#define NOTE_C7        11467 // 2093 Hz
#define NOTE_C7S       10825 // 2217 Hz
#define NOTE_D7        10217 // 2349 Hz
#define NOTE_D7S       9642 // 2489 Hz
#define NOTE_E7        9101 // 2637 Hz
#define NOTE_F7        8590 // 2794 Hz
#define NOTE_F7S       8108 // 2960 Hz
#define NOTE_G7        7653 // 3136 Hz
#define NOTE_G7S       7225 // 3322 Hz

//*****************************************************************************
// DO NOT MODIFY ANYTHING BELOW
//*****************************************************************************
#define MEASURE_DURATION 12000000    // 500 milliseconds
#define MEASURE_RATIO           2    // 2/4 time
#define DELAY_AMOUNT       240000    // 10  milliseconds
#define SONG_NUM_NOTES 13

typedef enum measure_time_t {
    ONE_QUARTER,
    ONE_HALF,
    ONE_EIGTH,
    THREE_EIGTH
} measure_time_t;

typedef struct{
    uint32_t period;
    measure_time_t time;
    bool delay;
}Note_t;

// **************************************************************
// Helper function to set the PWM pulse
// **************************************************************
void set_PWM_pulse(uint16_t period);

//***************************************************************
// Function to kick off and start playing each note in the song
//***************************************************************
void music_play_song(void);

#endif /* MUSIC_H_ */
