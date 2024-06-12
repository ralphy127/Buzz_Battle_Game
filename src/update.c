#include "./main.h"

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
                case SDLK_q: 
                    game_is_running = false;
                    break;
                case SDLK_ESCAPE:
                    game_is_paused = !game_is_paused;
                    if (!game_is_paused)
                        current_time = SDL_GetTicks();
                    break;
                case SDLK_w:
                case SDLK_UP:
                    player.y_direction = -1;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    player.y_direction = 1;
                    break;
                case SDLK_SPACE:
                    if(player.ammo > 0 && !game_is_paused) {
                        add_laser();
                        player.ammo--;
                    }
                    break;
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    player.y_direction = 0;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    player.y_direction = 0;
                    break;
            }
            break;
    }

    return;
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - current_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);
    float delta_time = (SDL_GetTicks() - current_time) / 1000.0f;
    current_time = SDL_GetTicks();

    relative_current_time = current_time - starting_time;

    if (current_time - last_insect_spawn_time >= INSECTS_INTERVAL) {
        spawn_insect();
        last_insect_spawn_time = current_time;
    }

    if(current_time - last_mini_boss_spawn_time >= MINI_BOSSES_INTERVAL) {
        spawn_mini_boss();
        last_mini_boss_spawn_time = current_time;
    }

    if (current_time - last_laser_time >= LASERS_INTRERVAL) {
        last_laser_time = current_time;
        if(player.ammo < player.max_ammo)
            player.ammo++;      
    }

    player.y += player.speed * player.y_direction * delta_time;

    if(player.y < 0)
        player.y = 0;
    else if(player.y + player.height > WINDOW_HEIGHT) 
        player.y = WINDOW_HEIGHT - player.height;

    if(player.health <= 0 || player.base_health <= 0)
        game_over = true;

    update_lasers(delta_time);
    update_insects(delta_time);
    update_insects_and_player_hit();
    update_mini_bosses(delta_time);
    update_mini_bosses_lasers(delta_time);
    update_mini_bosses_and_player_hit();

    return;
}

void add_laser(void) {
    lasers_t* new_laser = malloc(sizeof(lasers_t));
    if (!new_laser) 
        fprintf(stderr, "laser's memory allocation failed.\n");

    new_laser->next = lasers_head;
    new_laser->y = player.y + 0.45 * player.height;
    new_laser->x = player.x + 0.9 * player.width;
    new_laser->width = 2.51 * 0.015 * WINDOW_WIDTH;
    new_laser->height = 1.44 * 0.015 * WINDOW_WIDTH;
    new_laser->speed = LASER_SPEED;
    lasers_head = new_laser;

    Mix_PlayChannel(-1, sound_effects.player_laser, 0);

    return;
}

void update_lasers(float delta_time) {
    lasers_t* current = lasers_head;
    lasers_t* prev = NULL;

    while (current) {
        current->x += current->speed * delta_time;
        if (current->x > WINDOW_WIDTH) {
            lasers_t* to_delete = current;

            if (prev == NULL)
                lasers_head = current->next;
            else
                prev->next = current->next;

            current = current->next;
            free(to_delete);
        } 
        else {
            prev = current;
            current = current->next;
        }
    }

    return;
}

void spawn_insect(void) {
    insects_t* new_insect = malloc(sizeof(insects_t));
    if(!new_insect)
        fprintf(stderr, "Insects memory allocation failed.\n");

    new_insect->next = insects_head;
    new_insect->x = WINDOW_WIDTH;
    new_insect->y = rand() % (int)(0.75 * WINDOW_HEIGHT) + 0.05 * WINDOW_HEIGHT;
    int number_of_insects = 3;
    new_insect->multiplier = rand() % number_of_insects + 1;
    float size = 0.017;

    if(new_insect->multiplier == 3) {
        new_insect->width = 2.66 * size * WINDOW_WIDTH * 2.5;
        new_insect->height = 2.07 * size * WINDOW_WIDTH * 2.5;
    }
    else if(new_insect->multiplier == 2) {
        new_insect->width = 2.53 * size * WINDOW_WIDTH * 1.75;
        new_insect->height = 2.07 * size * WINDOW_WIDTH * 1.75;
    }
    else {
        new_insect->width = 2.73 * size * WINDOW_WIDTH;
        new_insect->height = 2.82 * size * WINDOW_WIDTH;
    }

    new_insect->speed = 0.07 * BASE_INSECTS_SPEED * ((number_of_insects - new_insect->multiplier) % number_of_insects + 1);
    new_insect->health = new_insect->multiplier;
    new_insect->player_hit = false;
    new_insect->last_insect_frame_time = SDL_GetTicks();
    new_insect->frame = 0;

    Mix_PlayChannel(-1, sound_effects.insects, 0);

    insects_head = new_insect;

    return;
}

void update_insects(float delta_time) {
    insects_t* current = insects_head;
    insects_t* prev = NULL;

    while(current) {
        current->speed += relative_current_time / 100000;
        current->x -= current->speed * delta_time;

        if(current->x - current->width < 0)
            player.base_health -= current->multiplier;

        if(current->health <= 0)
                player.score += current->multiplier * 10;

        if(current->x - current->width < 0 || current->health <= 0) {
            insects_t* to_delete = current;

            if(prev == NULL)
                insects_head = current->next;
            else
                prev->next = current->next;

            current = current->next;
            free(to_delete);
        }
        else {
            prev = current;
            current = current->next;
        }
    }

    return;
}

void update_insects_and_player_hit(void) {
    lasers_t* current_laser = lasers_head;
    lasers_t* prev_laser = NULL;
    insects_t* current_insect = insects_head;

    while(current_insect) {
        while(current_laser) {
            if (current_laser->x + current_laser->width >= current_insect->x && 
                current_laser->x + current_laser->width <= current_insect->x + current_insect->width &&
                current_laser->y + current_laser->height >= current_insect->y &&
                current_laser->y <= current_insect->y + current_insect->height) {

                Mix_PlayChannel(-1, sound_effects.insect_hit_impact, 0);
            
                lasers_t* laser_to_delete = current_laser;
                if(prev_laser == NULL)
                    lasers_head = current_laser->next;
                else 
                    prev_laser->next = current_laser->next;

                current_laser = current_laser->next;
                free(laser_to_delete);
                
                current_insect->health--;
            } 
            else {
                prev_laser = current_laser;
                current_laser = current_laser->next;
            }
        }

        if (current_insect->x <= player.x + player.width &&
            current_insect->x + current_insect->width >= player.x &&
            current_insect->y + current_insect->height >= player.y && 
            current_insect->y <= player.y + player.height &&
            !current_insect->player_hit) {
                Mix_PlayChannel(-1, sound_effects.player_hit_impact, 0);
                player.health -= current_insect->multiplier;
                current_insect->player_hit = true;
        }

        current_insect = current_insect->next;
        current_laser = lasers_head;
        prev_laser = NULL;
    }



    return;
}

void spawn_mini_boss(void) {
    mini_bosses_t* new = malloc(sizeof(mini_bosses_t));
    if(!new) 
        fprintf(stderr, "Mini boss's memory allocation failed");

    new->next = mini_bosses_head;

    new->width = 0.462 * 0.3 * WINDOW_WIDTH; // 462 × 336
    new->height = 0.336 * 0.3 * WINDOW_WIDTH;
    new->x = WINDOW_WIDTH;
    new->yi = rand() % (int)(0.75 * WINDOW_HEIGHT) + 0.05 * WINDOW_HEIGHT;
    new->y_deviation = 0.1 * WINDOW_HEIGHT;
    new->speed = 0.02 * WINDOW_WIDTH;
    new->health = 10;
    new->time_deviation = rand() % 20;
    new->frame = 0;
    new->last_laser_fired_time = SDL_GetTicks();

    mini_bosses_head = new;

    return;
}

void update_mini_bosses(float delta_time) {
    mini_bosses_t* current = mini_bosses_head;
    mini_bosses_t* prev = NULL;

    while(current) {
        if(current->x > 0.75 * WINDOW_WIDTH)
            current->x -= current->speed * delta_time;
            current->y = current->yi + current->y_deviation * sin(0.001 * current_time + current->time_deviation);
        
        if(current_time - current->last_laser_fired_time >= MINI_BOSSES_FIRING_INTERVAL) {
            mini_boss_lasers_t* new = malloc(sizeof(mini_boss_lasers_t));
            if(!new)
                fprintf(stderr, "Allocating mini boss's memory failed.\n");

            new->width = 2.51 * 0.015 * WINDOW_WIDTH;
            new->height = 1.44 * 0.015 * WINDOW_WIDTH;
            new->next = mini_bosses_lasers_head;
            new->x = current->x - new->width;
            new->y = current->y + 0.5 * current->height;
            new->speed = LASER_SPEED;
            new->player_hit = false;
            mini_bosses_lasers_head = new;

            Mix_PlayChannel(-1, sound_effects.mini_boss_laser, 0);
            
            current->last_laser_fired_time = SDL_GetTicks();
        }

        if(current->health <= 0) {
            mini_bosses_t* to_delete = current;

            if(prev == NULL)
                mini_bosses_head = current->next;
            else
                prev->next = current->next;

            current = current->next;
            free(to_delete);
        }
        else {
            prev = current;
            current = current->next;
        }
    }


    return;
}

void update_mini_bosses_lasers(float delta_time) {
    mini_boss_lasers_t* current = mini_bosses_lasers_head;
    mini_boss_lasers_t* prev = NULL;

    while (current) {
        current->x -= current->speed * delta_time;
        if (current->x + current->width < 0) {
            mini_boss_lasers_t* to_delete = current;

            if (prev == NULL)
                mini_bosses_lasers_head = current->next;
            else
                prev->next = current->next;

            current = current->next;
            free(to_delete);
        } 
        else {
            prev = current;
            current = current->next;
        }
    }

    return;
}

void update_mini_bosses_and_player_hit(void) {
    lasers_t* current_laser = lasers_head;
    lasers_t* prev_laser = NULL;
    mini_bosses_t* current_mini_boss = mini_bosses_head;

    while(current_mini_boss) {
        while(current_laser) {
            if (current_laser->x + current_laser->width >= current_mini_boss->x && 
                current_laser->x + current_laser->width <= current_mini_boss->x + current_mini_boss->width &&
                current_laser->y + current_laser->height >= current_mini_boss->y &&
                current_laser->y <= current_mini_boss->y + current_mini_boss->height) {
                
                Mix_PlayChannel(-1, sound_effects.mini_boss_hit_impact, 0);
            
                lasers_t* laser_to_delete = current_laser;
                if(prev_laser == NULL)
                    lasers_head = current_laser->next;
                else 
                    prev_laser->next = current_laser->next;

                current_laser = current_laser->next;
                free(laser_to_delete);
                
                current_mini_boss->health--;
            } 
            else {
                prev_laser = current_laser;
                current_laser = current_laser->next;
            }
        }

        current_mini_boss = current_mini_boss->next;
        current_laser = lasers_head;
        prev_laser = NULL;
    }

    mini_boss_lasers_t* current_mini_boss_laser = mini_bosses_lasers_head;
    mini_boss_lasers_t* prev_mini_boss_laser = NULL;

    while(current_mini_boss_laser) {
        if (current_mini_boss_laser->x <= player.x + player.width &&
            current_mini_boss_laser->x + current_mini_boss_laser->width >= player.x &&
            current_mini_boss_laser->y + current_mini_boss_laser->height >= player.y && 
            current_mini_boss_laser->y <= player.y + player.height &&
            !current_mini_boss_laser->player_hit) {

            Mix_PlayChannel(-1, sound_effects.player_hit_impact, 0);

            player.health--;
            current_mini_boss_laser->player_hit = true;

            mini_boss_lasers_t* laser_to_delete = current_mini_boss_laser;
            if(prev_mini_boss_laser == NULL)
                mini_bosses_lasers_head = current_mini_boss_laser->next;
            else 
                prev_mini_boss_laser->next = current_mini_boss_laser->next;

            current_mini_boss_laser = current_mini_boss_laser->next;
            free(laser_to_delete);
        }
        else 
            current_mini_boss_laser = current_mini_boss_laser->next;
    }

    return;
}