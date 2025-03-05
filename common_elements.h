#ifndef COMMON_ELEMENTS_H
#define COMMON_ELEMENTS_H

#define MAX_SHOTS_PER_ENEMY 4   // Each enemy can have up to 3 active shots

struct Projectile {
    int x_pos;
    int y_pos;
    int radius;
    int speed; // movement speed of projectile
    int type;
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


typedef struct {
    float x, y;
    float damage;
    int active;  // 1 if the shot is active, 0 if not
} Shot;

#endif // COMMON_ELEMENTS_H
