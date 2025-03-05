#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "scores.h"
#include "common_elements.h"

typedef enum {START_SCREEN, GAME_LOOP, SCOREBOARD} GameStates;

//--------------------------------------------------------------------

// return 0 if still on screen, 1 if entering game_loop,
int Run_StartScreen() {
    // display screen

    // if ENTER key || Button1 in DataBuffer[], InitGame() and output 1.. unless Name is Empty

    // otherwise update playerName buffer based on IR inputs from DataBuffer[]

    return 0;
}

// return 0 if game is running, 1 if it ended
int Run_GameLoop() {
    // move player & shots

    // move enemies & shots

    // detect enemy collisions

    // detect player collisions

    // is game running? return 1 if not

    return 0;
}

// return 0 if still on scoreboard, 1 if restarting game, 2 going to start screen
int Run_ScoreBoard() {
    // display player score

    // display scoreboard

    // if user requests save, StoreScore() and SendScoreToAWS()

    // if Button1, return 1;
    // if LAST on IR, return 2;

    return 0;
}


#endif
