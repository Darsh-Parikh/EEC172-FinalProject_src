#ifndef ENEMY_H
#define ENEMY_H

#include "common_elements.h"

#define MAX_ENEMIES 5
#define NUM_SPAWN_POINTS 3
#define SPAWN_COOLDOWN 5.0f     // New enemy every 5 seconds
#define RELOAD_RATE 3.0f        // Enemy can fire every 3 seconds
#define SHOT_SPEED 2.0f         // Speed factor for bullets
#define MAX_SHOTS_PER_ENEMY 3   // Each enemy can have up to 3 active shots
#define MAX_PLAYER_SHOTS 10     // Player can have multiple active shots
#define ENEMY_HIT_RADIUS 10.0f  // Collision detection radius
#define PLAYER_SHOT_DAMAGE 50   // Damage per shot

typedef enum { RED, GREEN, BLUE, YELLOW } Color;
typedef enum { STANDARD, FAST, BOSS } EnemyType;

typedef struct {
    float x, y;
    float timeSinceLastSpawn;
} SpawnPoint;

//-----------------------------------------------------------

// List of spawn points
SpawnPoint spawnPoints[NUM_SPAWN_POINTS];

// Enemy list
Enemy enemies[MAX_ENEMIES];
int num_enemies;
float timeSinceLastSpawn = 0.0f;

//-----------------------------------------------------------

/**
 * Initialize enemies and player shots.
 */
void InitEnemySystem(int minSpawnPoint, int maxSpawnPoint) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0; // No active enemies at start
        for (int j = 0; j < MAX_SHOTS_PER_ENEMY; j++) {
            enemies[i].shots[j].active = 0; // No shots fired yet
        }
    }
    num_enemies = 0;

    // initialize SPAWN LOCATIONS

}

void SpawnEnemies(int topHeight) {
    if (num_enemies = MAX_ENEMIES) {
        return;
    }

    // ---

}

void MoveEnemies() {
    //
}

/**
 * Check for collisions between player shots and enemies.
 */
void CheckEnemyCollisions(Projectile *playerShots) {
    for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
        if (!playerShots[i].active) {
            continue;
        }
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) {
                continue;
            }
            float dx = playerShots[i].x - enemies[j].x;
            float dy = playerShots[i].y - enemies[j].y;
            float distanceSquared = dx * dx + dy * dy;
            float hitRadiusSquared = ENEMY_HIT_RADIUS * ENEMY_HIT_RADIUS;

            if (distanceSquared <= hitRadiusSquared) { // Collision detected
                enemies[j].health -= PLAYER_SHOT_DAMAGE;
                playerShots[i].active = 0; // Shot disappears

                if (enemies[j].health <= 0) {
                    enemies[j].active = 0; // Enemy is defeated
                    num_enemies--;
                }
            }
        }
    }
}

#endif
