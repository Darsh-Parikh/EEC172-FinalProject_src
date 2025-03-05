/*
 * console_display.h
 *
 *  Created on: 4 Mar 2025
 *      Author: mrcol
 */

#ifndef EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_
#define EEC172_FINALPROJECT_SRC_CONSOLE_DISPLAY_H_

#include "enemy.h"
#include "common_elements.h"
#include "accelerometer.h"

#define PLAYER_TYPE 0
#define PLAYER_PROJECTILE 0

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

volatile int tick_count = 0;
int tick_time = 80000000/100;

void tickIncrement() {
    MAP_UtilsDelay(tick_time);
    tick_count++;
}

int findNextShot(Entity *entity) {
    int index = 0;
    int num_found = 0;
    while(num_found <= entity->num_shots && index < MAX_SHOTS_PER_ENEMY) {
        if (num_found == entity->num_shots) return index;
        if (entity->shots[index].active) num_found++;
        index++;
    }
    return -1;
}

int shoot(Entity *entity) {
    int index = findNextShot(entity);
    if(index == -1) return -1;
    entity->shots[index].x_pos = entity->x_pos;
    if(entity->type == PLAYER_TYPE) {
        entity->shots[index].y_pos = entity->y_pos - entity->radius;
    } else {}
    entity->shots[index].active = 1;
    entity->shots[index].time_since_spawn = 0;
    return 0;
}

void shotInit(Entity *entity) {
    int i = 0;
    for(i; i < MAX_SHOTS_PER_ENEMY; i++) {
        if(entity->type == PLAYER_TYPE){
            entity->shots[i].radius = 2;
            entity->shots[i].speed = 4;
            entity->shots[i].type = PLAYER_PROJECTILE;
            entity->shots[i].color = Color565(200,0,0);
            entity->shots[i].active = 0;
        }
    }
    return;
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

void moveShots(Entity *entity) {
    int i = 0;
    int num_moved = 0;
    for(i; i < MAX_SHOTS_PER_ENEMY; i++) {
        if(!entity->shots[i].active) continue;
        if(num_moved >= entity->num_shots) break;
        num_moved++;
        if(entity->type == PLAYER_TYPE){
            entity->shots[i].y_pos -= entity->shots[i].speed;
        }
        if((entity->shots[i].y_pos > 125 - entity->shots[i].radius) || (entity->shots[i].y_pos < entity->shots[i].radius + 1)) {
            entity->shots[i].active = 0;
            entity->num_shots--;
        }
    }
    return;
}

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
