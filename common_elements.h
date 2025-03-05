#ifndef COMMON_ELEMENTS_H
#define COMMON_ELEMENTS_H

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
};
typedef struct Entity Entity;

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


typedef struct {
    float x, y;
    float damage;
    int active;  // 1 if the shot is active, 0 if not
} Shot;

#endif // COMMON_ELEMENTS_H
