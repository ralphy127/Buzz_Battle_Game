#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "./constants.h"
#include "./init.h"
#include "./update.h"
#include "./render.h"
#include "./cleanup.h"

struct {
    float x;
    float y;
    float width;
    float height;
    float y_direction;
    float speed;
    int health;
    int base_health;
    int max_ammo;
    int ammo;
    int score;
    int best_score;
} player;

typedef struct lasers {
    float x;
    float y;
    float width;
    float height;
    float speed;
    struct lasers* next;
} lasers_t;
extern lasers_t* lasers_head;

typedef struct insects {
    float x;
    float y;
    float width;
    float height;
    float speed;
    int health;
    int multiplier;
    unsigned int last_insect_frame_time;
    int frame;
    bool player_hit;
    struct insects* next;
} insects_t;
extern insects_t* insects_head;

typedef struct mini_bosses {
    float x;
    float yi;
    float y_deviation;
    float y;
    float width;
    float height;
    float speed;
    int health;
    unsigned int last_laser_fired_time;
    int time_deviation;
    int frame;
    struct mini_bosses* next;
} mini_bosses_t;
extern mini_bosses_t* mini_bosses_head;

typedef struct mini_bosses_lasers {
    float x;
    float y;
    float width;
    float height;
    float speed;
    bool player_hit;
    struct mini_bosses_lasers* next;
} mini_boss_lasers_t;
extern mini_boss_lasers_t* mini_bosses_lasers_head;

struct {
    SDL_Texture* purple_bug[15]; // 266 × 207
    SDL_Texture* red_bug[15]; // 253 × 207 
    SDL_Texture* bee[13]; // 273 × 282 
    SDL_Texture* aircraft; // 480 × 333
    SDL_Texture* background;
    SDL_Texture* laser; // 251 × 144 px
    SDL_Texture* red_heart;
    SDL_Texture* yellow_heart;
    SDL_Texture* mini_boss; // 462 × 336
    SDL_Texture* mini_boss_laser;
} textures;

struct {
    Mix_Chunk* background_sounds; // 1:02,06
    Mix_Chunk* player_laser;
    Mix_Chunk* insects;
    Mix_Chunk* mini_boss_laser;
    Mix_Chunk* player_hit_impact;
    Mix_Chunk* insect_hit_impact;
    Mix_Chunk* mini_boss_hit_impact;
} sound_effects;

extern bool game_is_running;
extern bool game_started;
extern bool game_is_paused;
extern bool game_over;
extern bool muted;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;

extern unsigned int starting_time;
extern unsigned int current_time;
extern unsigned int relative_current_time;
extern unsigned int last_insect_spawn_time;
extern unsigned int last_mini_boss_spawn_time;
extern unsigned int last_laser_time;

#endif