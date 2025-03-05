#ifndef COMMON_ELEMENTS_H
#define COMMON_ELEMENTS_H

#define MAX_SHOTS_PER_ENEMY 3   // Each enemy can have up to 3 active shots

struct Projectile {
    int x_pos;
    int y_pos;
    int radius;
    int speed;
    int type;
    unsigned int color;
    int time_since_spawn;
    int active;
};
typedef struct Projectile Projectile;

struct Entity {
   int x_pos;
   int y_pos;
   int length;
   int height;
   int radius;
   int health;
   int stored_effect;
   int active_effect;
   unsigned int color;
   int type;
   int time_since_fire;
   int time_since_spawn;
   int active;
   Projectile shots[MAX_SHOTS_PER_ENEMY];
};
typedef struct Entity Entity;


typedef struct {
    float x, y;
    float damage;
    int active;  // 1 if the shot is active, 0 if not
} Shot;

#endif // COMMON_ELEMENTS_H
