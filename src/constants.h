#ifndef CONSTANTS_H
#define CONSTANTS_H

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
#define BACKGROUND_WIDTH (int)(2550 * WINDOW_WIDTH / 1440)

#define PLAYER_SPEED (0.7 * WINDOW_HEIGHT)
#define LASER_SPEED (0.8 * WINDOW_WIDTH)
#define BASE_INSECTS_SPEED (0.6 * WINDOW_WIDTH)

#define INSECTS_INTERVAL 1444
#define MINI_BOSSES_INTERVAL 10000
#define MINI_BOSSES_FIRING_INTERVAL 3000
#define LASERS_INTRERVAL 350
#define ANIMATION_INTERVAL 60

#define SHOW_HITBOXES false
#define MASTER_VOLUME 5 // scale 0-10

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

#endif