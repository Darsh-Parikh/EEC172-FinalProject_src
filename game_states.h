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
    int i = 0;

    clearEntity(&player);
    for (i = 0; i < MAX_ENEMIES; i++) {
        clearEntity(&(enemies[i]));
    }

    // get accelX and accelY
    MovePlayerAndShots(0, 0);
    MoveEnemiesAndAddShots();

    int ended = CheckCollisions();
    if (ended) {
        return 1;
    }

    SpawnEnemies();

    if (0 /* Button 1 */) {
        PlayerShoot();
    }

    drawEntity(&player);
    for (i = 0; i < MAX_ENEMIES; i++) {
        drawEntity(&(enemies[i]));
    }

    for (i = 0; i < NUM_SPAWN_POINTS; i++) {
        drawCircle(spawnPoints[i].x, spawnPoints[i].y, 1, RED);
    }

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

int InitGame() {
    InitPlayer();
    InitEnemySystem(5, 122);

    ClearBuffer(&playerName);
    playerScore = 0;

    drawRect(0,0,127,127,Color565(100,100,100));

    return 0;
}


#endif
