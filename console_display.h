/*
 * console_display.h
 *
 *  Created on: 4 Mar 2025
 *      Author: mrcol
 */

#ifndef EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_
#define EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_

#include enemy.h
#include common_elements.h

//****************************************************************************
//************************  Ball Motion Functions ****************************
//****************************************************************************

#define X_Accel_PixelLimit 5
#define Y_Accel_PixelLimit 5

void MoveBall(int ballLoc[2], int ballVel[2], int ballAccel[2], int includeVel) {
    int timeDelay = 1;

    ballVel[0] += ballAccel[0] * timeDelay;
    ballVel[1] += ballAccel[1] * timeDelay;

    ballLoc[0] += includeVel*(ballVel[0] * timeDelay) + (ballAccel[0] * timeDelay * timeDelay);
    ballLoc[1] += includeVel*(ballVel[1] * timeDelay) + (ballAccel[1] * timeDelay * timeDelay);
}

int ConstrainBall(int ballLoc[2], int x_lim, int y_lim) {
    int x = ballLoc[0], y = ballLoc[1];

    int retVal = 0;

    if (x < (-1 * x_lim)) {
        ballLoc[0] = (-1 * x_lim);
        retVal = 1;
    } else if (x > x_lim) {
        ballLoc[0] = x_lim;
        retVal = 1;
    }

    if (y < (-1 * y_lim)) {
        ballLoc[1] = (-1 * y_lim);
        retVal = 1;
    } else if (y > y_lim) {
        ballLoc[1] = y_lim;
        retVal = 1;
    }

    return retVal;
}

//-----------------------------------------------------------------------------

void drawEntity(Entity *entity) {
    if (entity->type == PLAYER_TYPE) {
        fillCircle(entity->x_pos,entity->y_pos,entity->radius,entity->color);
    }
}

void clearEntity(Entity *entity) {
    if (entity->type == PLAYER_TYPE) {
        fillCircle(entity->x_pos,entity->y_pos,entity->radius,BLACK);
    }
}

void drawText(int x, int y, char* text, int color, int bg, int size){
    int i = 0;
    while(text[i] != '\0'){
        drawChar(x, y, text[i], color, bg, size);
        x += 8;
        i++;
        if(x>=120){
            y += 8;
            x = 0;
        }
    }
}
void drawFont(int x, int y, int color, int bg, int size){
    int i = 0;
    while(i < sizeof(font)/8){
        drawChar(x, y, i, color, bg, size);
        x += 8;
        i++;
        if(x>=120){
            y += 8;
            x = 0;
        }
    }
}

int display() {
    struct Entity player;
    player.type = PLAYER_TYPE;
    player.x_pos = 64;
    player.y_pos = 64;
    player.radius = 4;
    player.color = Color565(000,200,200);
    int ballLoc[2] = {64,64};
    int ballVel[2] = {0,0};
    drawRect(0,0,127,127,Color565(100,100,100));
    while(1) {
        drawEntity(player);
        tickIncrement();
        clearEntity(player);
        int ballAccel[2] = {
            GetAccel(X_Axis, X_Accel_PixelLimit),
            GetAccel(Y_Axis, Y_Accel_PixelLimit)
        };
        MoveBall(ballLoc, ballVel, ballAccel, 0);
        player.x_pos = ballLoc[0];
        player.y_pos = ballLoc[1];
        if(player.x_pos < player.radius + 1) player.x_pos = player.radius + 1;
        if(player.x_pos > 125 - player.radius) player.x_pos = 125 - player.radius;
        if(player.y_pos < player.radius + 1) player.y_pos = player.radius + 1;
        if(player.y_pos > 125 - player.radius) player.y_pos = 125 - player.radius;
    }
}

#endif /* EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_ */
