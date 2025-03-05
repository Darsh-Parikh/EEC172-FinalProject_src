#ifndef COMMON_ELEMENTS_H
#define COMMON_ELEMENTS_H

#include "scores.h"
#include <stdlib.h>

//------------------------------------------------------------

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

//--------------------------------------------------------------

#define MAX_SHOTS_PER_ENEMY 4   // Each enemy can have up to 3 active shots

#define PLAYER_TYPE 0
#define PLAYER_PROJECTILE 0

#define ENEMY_TYPE 1
#define ENEMY_PROJECTILE 1


struct Projectile {
    int x_pos;
    int y_pos;
    int radius;
    int speed; // movement speed of projectile
    int type;
    int damage;
    unsigned int color;
    int time_since_spawn; // ticks since projectile was spawned
    int active;
};
typedef struct Projectile Projectile;

struct Entity {
   int x_pos;
   int y_pos;
   int length; // only used for specific shapes
   int height; // only used for specific shapes
   int radius; // only used for specific shapes
   int health;
   int stored_effect; // power up stored
   int active_effect; // power up currently active
   unsigned int color;
   int type; // player or type of enement
   int time_since_fire; // ticks since last shot
   int time_since_spawn; // ticks since last spawn
   int active; // is entity active or dead
   int num_shots; // number of active shots
   int fire_rate;
   Projectile shots[MAX_SHOTS_PER_ENEMY];
};
typedef struct Entity Entity;

//-----------------------------------------------------------------------

// returns index of next shot to be activated
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

// activate the next shot
int shoot(Entity *entity) {
    int index = findNextShot(entity);
    if(index == -1) return -1;
    entity->shots[index].x_pos = entity->x_pos;

    if(entity->type == PLAYER_TYPE) {
        entity->shots[index].y_pos = entity->y_pos - entity->radius;
    } else if (entity->type == ENEMY_TYPE) {
        entity->shots[index].y_pos = entity->y_pos + entity->radius;
    }

    entity->shots[index].active = 1;
    entity->shots[index].time_since_spawn = 0;
    return 0;
}

// initialize all the shots
void shotInit(Entity *entity) {
    int i = 0;
    for(i; i < MAX_SHOTS_PER_ENEMY; i++) {
        if(entity->type == PLAYER_TYPE){
            entity->shots[i].type = PLAYER_PROJECTILE;
            entity->shots[i].color = Color565(200,0,0);
        } else if (entity->type == ENEMY_TYPE) {
            entity->shots[i].type = ENEMY_PROJECTILE;
            entity->shots[i].color = Color565(200,0,0);
        }

        entity->shots[i].radius = 2;
        entity->shots[i].speed = 4;
        entity->shots[i].damage = 10;
        entity->shots[i].active = 0;
    }
    return;
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
        } else if (entity->type == ENEMY_TYPE) {
            entity->shots[i].y_pos += entity->shots[i].speed;
        }

        if((entity->shots[i].y_pos > 125 - entity->shots[i].radius) || (entity->shots[i].y_pos < entity->shots[i].radius + 1)) {
            entity->shots[i].active = 0;
            entity->num_shots--;
        }
    }
    return;
}

//-----------------------------------------------------

#define MAX_ENEMIES 5
#define NUM_SPAWN_POINTS 3
#define SPAWN_COOLDOWN 5.0f     // New enemy every 5 seconds
#define RELOAD_RATE 3.0f        // Enemy can fire every 3 seconds

typedef struct {
    float x, y;
    float timeSinceLastSpawn;
} SpawnPoint;

//-----------------------------------------------------------

// List of spawn points
SpawnPoint spawnPoints[NUM_SPAWN_POINTS];

// Enemy list
Entity enemies[MAX_ENEMIES];
int num_enemies;
float timeSinceLastSpawn = 0.0f;

//-----------------------------------------------------------

void SpawnEnemy(Entity *enemy, int start_x, int start_y) {
   enemy->x_pos = start_x;
   enemy->y_pos = start_y;
   enemy->radius = 3;
   enemy->health = 100;
   enemy->fire_rate = 20;
   enemy->time_since_fire = 0;
   enemy->num_shots = 0;
   enemy->active = 1;
   enemy->type = ENEMY_TYPE;
   enemy->color = BLUE;
   shotInit(enemy);
}

/**
 * Initialize enemies and player shots.
 */
void InitEnemySystem(int minSpawnPoint, int maxSpawnPoint) {
    int i =0;
    for (i = 0; i < MAX_ENEMIES; i++) {
        SpawnEnemy(&(enemies[i]), 0, 0);
        enemies[i].active = 0;
    }
    num_enemies = 0;

    // initialize SPAWN LOCATIONS
    float xSpacing = (maxSpawnPoint - minSpawnPoint) / (NUM_SPAWN_POINTS - 1);
    for (i = 0; i < NUM_SPAWN_POINTS; i++) {
        spawnPoints[i].x = minSpawnPoint + i * xSpacing;
        spawnPoints[i].y = 127;
        spawnPoints[i].timeSinceLastSpawn = 0.0f;
    }

}

void SpawnEnemies() {
    if (num_enemies == MAX_ENEMIES) {
        return;
    }

    float spawnThreshold = 3.0f;  // Minimum time since last spawn to be considered valid
    int validSpawnPoints[NUM_SPAWN_POINTS];
    int validCount = 0;

    // Find spawn points that are above the threshold
    int i = 0;
    for (i = 0; i < NUM_SPAWN_POINTS; i++) {
        if (spawnPoints[i].timeSinceLastSpawn >= spawnThreshold) {
            validSpawnPoints[validCount++] = i;
        }
    }
    if (validCount == 0) return;

    // Choose a random valid spawn point
    int chosenIndex = validSpawnPoints[rand() % validCount];

    // Find an available enemy slot
    int enemyIndex = -1;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemyIndex = i;
            break;
        }
    }
    if (enemyIndex == -1) return; // No available enemy slots

    SpawnEnemy(&(enemies[enemyIndex]), spawnPoints[chosenIndex].x, spawnPoints[chosenIndex].y);

    spawnPoints[chosenIndex].timeSinceLastSpawn = 0.0f;
    num_enemies++;
}

void MoveEnemies() {
    int i = 0;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            int enemyLoc[2] = {enemies[i].x_pos, enemies[i].y_pos};
            int enemyVel[2] = {0, 0};
            int enemyAccel[2] = {0, 1};

            MoveBall(enemyLoc, enemyVel, enemyAccel, 0);

            enemies[i].x_pos = enemyLoc[0];
            enemies[i].y_pos = enemyLoc[1];

            // Boundary checking
            if (enemies[i].x_pos < enemies[i].radius + 1) enemies[i].x_pos = enemies[i].radius + 1;
            if (enemies[i].x_pos > 125 - enemies[i].radius) enemies[i].x_pos = 125 - enemies[i].radius;
            if (enemies[i].y_pos > 125 - enemies[i].radius) enemies[i].y_pos = 125 - enemies[i].radius;

            // Enemy shooting logic
            enemies[i].time_since_fire++;
            if (enemies[i].num_shots < MAX_SHOTS_PER_ENEMY && enemies[i].time_since_fire >= (100 / enemies[i].fire_rate)) {
                if (!shoot(&enemies[i])) {
                    enemies[i].num_shots++;
                    enemies[i].time_since_fire = 0;
                }
            }

            moveShots(&enemies[i]);  // Move enemy shots
        }
    }
}

/**
 * Check for collisions between player shots and enemies.
 */
void CheckEnemyCollisions(Projectile *playerShots) {
    int i = 0;
    for (i = 0; i < MAX_SHOTS_PER_ENEMY; i++) {
        if (!playerShots[i].active) {
            continue;
        }
        int j = 0;
        for (j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) {
                continue;
            }
            float dx = playerShots[i].x_pos - enemies[j].x_pos;
            float dy = playerShots[i].y_pos - enemies[j].y_pos;
            float distanceSquared = dx * dx + dy * dy;
            float hitRadiusSquared = enemies[j].radius * enemies[j].radius;

            if (distanceSquared <= hitRadiusSquared) { // Collision detected
                enemies[j].health -= playerShots[i].damage;
                playerShots[i].active = 0; // Shot disappears

                if (enemies[j].health <= 0) {
                    enemies[j].active = 0; // Enemy is defeated
                    num_enemies--;
                    IncreaseScore(10);
                }
            }
        }
    }
}


#endif // COMMON_ELEMENTS_H
