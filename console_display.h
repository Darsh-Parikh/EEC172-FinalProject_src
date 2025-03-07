/*
 * console_display.h
 *
 *  Created on: 4 Mar 2025
 *      Author: mrcol
 */

#ifndef EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_
#define EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_

#include "timing.h"
#include "common_elements.h"
#include "accelerometer.h"

//****************************************************************************
//************************  Ball Motion Functions ****************************
//****************************************************************************

#define X_Accel_PixelLimit 5
#define Y_Accel_PixelLimit 5

//-----------------------------------------------------------------------------

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

void drawShots(Entity *entity) {
    int i = 0;
    if(!entity->num_shots) return;
    for(i; i < MAX_SHOTS_PER_ENEMY; i++){
        if(entity->shots[i].active){
            fillCircle(entity->shots[i].x_pos,entity->shots[i].y_pos,entity->shots[i].radius,entity->shots[i].color);
        }
    }
}

void clearShots(Entity *entity) {
    int i = 0;
    if(!entity->num_shots) return;
    for(i; i < MAX_SHOTS_PER_ENEMY; i++){
        if(entity->shots[i].active){
            fillCircle(entity->shots[i].x_pos,entity->shots[i].y_pos,entity->shots[i].radius,BLACK);
        }
    }
}

void drawEntity(Entity *entity) {
    fillCircle(entity->x_pos,entity->y_pos,entity->radius,entity->color);
    drawShots(entity);
}

void clearEntity(Entity *entity) {
    fillCircle(entity->x_pos,entity->y_pos,entity->radius,BLACK);
    clearShots(entity);
}

//----------------------------------------------------------

int display() {
    struct Entity player;
    player.type = PLAYER_TYPE;
    player.x_pos = 64;
    player.y_pos = 100;
    player.radius = 4;
    player.color = Color565(000,200,200);
    player.num_shots = 0;
    player.fire_rate = 25;
    player.time_since_fire = 0;
    shotInit(&player);
    int ballLoc[2] = {64,100};
    int ballVel[2] = {0,0};
    drawRect(0,0,127,127,Color565(100,100,100));
    while(1) {
        drawEntity(&player);
        drawShots(&player);
        tickIncrement();
        clearEntity(&player);
        clearShots(&player);
        int ballAccel[2] = {
            GetAccel(X_Axis, X_Accel_PixelLimit),
            GetAccel(Y_Axis, Y_Accel_PixelLimit)
        };
        MoveBall(ballLoc, ballVel, ballAccel, 0);
        player.x_pos = ballLoc[0];
        player.y_pos = ballLoc[1];
        player.time_since_fire++;
        if(player.x_pos < player.radius + 1) player.x_pos = player.radius + 1;
        if(player.x_pos > 125 - player.radius) player.x_pos = 125 - player.radius;
        if(player.y_pos < player.radius + 1) player.y_pos = player.radius + 1;
        if(player.y_pos > 125 - player.radius) player.y_pos = 125 - player.radius;
        if(player.num_shots <= MAX_SHOTS_PER_ENEMY && player.time_since_fire >= (100 / player.fire_rate)) {
            if(!shoot(&player)) {
                player.num_shots++;
                player.time_since_fire = 0;
            }
        }
        moveShots(&player);
        //Report("%d",player.num_shots);
    }
}

#endif /* EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_ */
