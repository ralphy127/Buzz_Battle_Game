#include "./main.h"

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect window_rect = {
        -(BACKGROUND_WIDTH - WINDOW_WIDTH) * 0.5,
        0,
        WINDOW_WIDTH + (BACKGROUND_WIDTH - WINDOW_WIDTH) * 0.5,
        WINDOW_HEIGHT
    };
    SDL_RenderCopyEx(renderer, textures.background, NULL, &window_rect, 0, NULL, SDL_FLIP_NONE);

    float health_x = 0.01 * WINDOW_WIDTH;
    float health_y = 0.035 * WINDOW_HEIGHT;
    float health_width = 0.03 * WINDOW_WIDTH;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for(int i=0; i<player.health; i++) {
        SDL_Rect health_rect = {
            health_x,
            health_y,
            health_width,
            health_width
        };
        SDL_RenderCopyEx(renderer, textures.red_heart, NULL, &health_rect, 0, NULL, SDL_FLIP_NONE);
        health_x += 1.2 * health_width;
    }

    health_x = 0.01 * WINDOW_WIDTH;
    health_y = 0.09 * WINDOW_HEIGHT;
    for(int i=0; i<player.base_health; i++) {
        SDL_Rect health_rect = {
            health_x,
            health_y,
            health_width,
            health_width
        };
        SDL_RenderCopyEx(renderer, textures.yellow_heart, NULL, &health_rect, 0, NULL, SDL_FLIP_NONE);
        health_x += 1.2 * health_width;
    }

    float ammo_x = 0.03 * WINDOW_WIDTH;
    float ammo_y = 0.92 * WINDOW_HEIGHT;
    for(int i=0; i<player.ammo; i++) {
        SDL_Rect ammo_rect = {
            ammo_x,
            ammo_y,
            0.02 * WINDOW_WIDTH,
            0.08 * WINDOW_HEIGHT
        };
        SDL_RenderCopyEx(renderer, textures.laser, NULL, &ammo_rect, -90, NULL, SDL_FLIP_NONE);
        ammo_x += 1.5 * 0.01 * WINDOW_WIDTH;
    }

    SDL_Color grey = {224, 224, 224, 255};

    char score_text[64];
    sprintf(score_text, "Score: %d", player.score);
    SDL_Surface* surface_score = TTF_RenderText_Solid(font, score_text, grey);
    SDL_Texture* score = SDL_CreateTextureFromSurface(renderer, surface_score);
    int score_length = strlen(score_text);
    float score_width = 0.009 * WINDOW_WIDTH * score_length;
    float score_height = 0.04 * WINDOW_HEIGHT;
    SDL_Rect score_rect = {
        (int)(0.65 * WINDOW_WIDTH),
        (int)(0.03 * WINDOW_HEIGHT),
        (int)(score_width),
        (int)(score_height)
    };
    SDL_RenderCopy(renderer, score, NULL, &score_rect);
    SDL_FreeSurface(surface_score);
    SDL_DestroyTexture(score);

    char best_score_text[64];
    sprintf(best_score_text, "Best score: %d", player.best_score);
    SDL_Surface* surface_best_score = TTF_RenderText_Solid(font, best_score_text, grey);
    SDL_Texture* best_score = SDL_CreateTextureFromSurface(renderer, surface_best_score);
    int best_score_length = strlen(best_score_text);
    float best_score_width = 0.009 * WINDOW_WIDTH * best_score_length;
    float best_score_height = 0.04 * WINDOW_HEIGHT;
    SDL_Rect best_score_rect = {
        (int)(0.8 * WINDOW_WIDTH),
        (int)(0.03 * WINDOW_HEIGHT),
        (int)(best_score_width),
        (int)(best_score_height)
    };
    SDL_RenderCopy(renderer, best_score, NULL, &best_score_rect);
    SDL_FreeSurface(surface_best_score);
    SDL_DestroyTexture(best_score);

    render_insects();
    render_mini_bosses();

    SDL_Rect player_rect = {
        (int)player.x,
        (int)player.y,
        (int)player.width,
        (int)player.height
    };
    SDL_RenderCopyEx(renderer, textures.aircraft, NULL, &player_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
    if(SHOW_HITBOXES) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &player_rect);
    }
        
    lasers_t* current_laser = lasers_head;
    while (current_laser) {
        SDL_Rect laser_rect = {
            current_laser->x,
            current_laser->y,
            current_laser->width,
            current_laser->height
        };
        if(SHOW_HITBOXES) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &laser_rect);
        }
        SDL_RenderCopyEx(renderer, textures.laser, NULL, &laser_rect, 0, NULL, SDL_FLIP_NONE);
        current_laser = current_laser->next;
    }

    mini_boss_lasers_t* current_mini_bosses_laser = mini_bosses_lasers_head;
    while (current_mini_bosses_laser) {
        SDL_Rect mini_bosses_laser = {
            current_mini_bosses_laser->x,
            current_mini_bosses_laser->y,
            current_mini_bosses_laser->width,
            current_mini_bosses_laser->height
        };
        if(SHOW_HITBOXES) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &mini_bosses_laser);
        }
        SDL_RenderCopyEx(renderer, textures.mini_boss_laser, NULL, &mini_bosses_laser, 0, NULL, SDL_FLIP_HORIZONTAL);
        current_mini_bosses_laser = current_mini_bosses_laser->next; 
    }

    if(game_is_paused) 
        render_pause_screen();
    if(!game_started)
        render_start_screen();
    if(game_over)
        render_end_screen();
    
    SDL_RenderPresent(renderer); 

    return;
}

void render_insects(void) {
    insects_t* current = insects_head;
    
    while(current) {

        if(current_time - current->last_insect_frame_time >= ANIMATION_INTERVAL && !game_is_paused && !game_over) {
            current->frame++;
            current->last_insect_frame_time = current_time;
        }

        if((current->multiplier == 2 || current->multiplier == 3) && current->frame % 14 == 0)
            current->frame = 0;
        else if(current->multiplier == 1 && current->frame % 12 == 0)
            current->frame = 0;

        SDL_Texture *texture;
        SDL_RendererFlip flip;
        if(current->multiplier == 3) {
            flip = SDL_FLIP_HORIZONTAL;
            texture = textures.purple_bug[current->frame];
        }
        else if(current->multiplier == 2) {
            flip = SDL_FLIP_HORIZONTAL;
            texture = textures.red_bug[current->frame];
        }
        else {
            flip = SDL_FLIP_NONE;
            texture = textures.bee[current->frame];
        }

        SDL_Rect insect_rect = {
            current->x,
            current->y,
            current->width,
            current->height
        };
        SDL_RenderCopyEx(renderer, texture, NULL, &insect_rect, 0, NULL, flip);
        if(SHOW_HITBOXES) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &insect_rect);
        }

        current = current->next;
    }

    return;
}

void render_mini_bosses(void) {
    mini_bosses_t* current = mini_bosses_head;
    
    while(current) {
        SDL_Texture *texture;
        texture = textures.mini_boss;

        SDL_Rect mini_boss_rect = {
            current->x,
            current->y,
            current->width,
            current->height
        };
        SDL_RenderCopyEx(renderer, texture, NULL, &mini_boss_rect, 0, NULL, SDL_FLIP_NONE);
        if(SHOW_HITBOXES) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &mini_boss_rect);
        }

        current = current->next;
    }

    return;
}

void render_start_screen(void) {
    float background_height = WINDOW_HEIGHT * 0.3;
    SDL_Rect background_rect = {
        0,
        (int)(0.5 * WINDOW_HEIGHT - 0.5 * background_height),
        WINDOW_WIDTH,
        (int)(background_height)
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &background_rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color grey = {224, 224, 224, 255};

    SDL_Surface* surface_game_name = TTF_RenderText_Solid(font, "Buzz Battle: Space Wars", grey);
    SDL_Texture* game_name = SDL_CreateTextureFromSurface(renderer, surface_game_name);
    float game_name_width = 0.8 * WINDOW_WIDTH;
    float game_name_height = 0.8 * background_height;
    SDL_Rect game_name_rect = {
        (int)(0.5 * WINDOW_WIDTH - 0.5 * game_name_width),
        (int)(0.5 * WINDOW_HEIGHT - 0.65 * game_name_height),
        (int)(game_name_width),
        (int)(game_name_height)
    };
    SDL_RenderCopy(renderer, game_name, NULL, &game_name_rect);

    SDL_Surface* surface_press_message = TTF_RenderText_Solid(font, "Press any key to start", grey);
    SDL_Texture* press_message = SDL_CreateTextureFromSurface(renderer, surface_press_message);
    float press_message_width = 0.28 * WINDOW_WIDTH;
    float press_message_height = 0.17 * background_height;
    SDL_Rect press_message_rect = {
        (int)(0.5 * WINDOW_WIDTH - 0.5 * press_message_width),
        (int)(0.5 * WINDOW_HEIGHT + 0.3 * game_name_height),
        (int)(press_message_width),
        (int)(press_message_height)
    };
    SDL_RenderCopy(renderer, press_message, NULL, &press_message_rect);

    SDL_FreeSurface(surface_game_name);
    SDL_FreeSurface(surface_press_message);
    SDL_DestroyTexture(game_name);
    SDL_DestroyTexture(press_message);

    return;
}

void render_pause_screen(void) {
    float background_height = WINDOW_HEIGHT * 0.3;
    SDL_Rect background_rect = {
        0,
        (int)(0.5 * WINDOW_HEIGHT - 0.5 * background_height),
        WINDOW_WIDTH,
        (int)(background_height)
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &background_rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color grey = {224, 224, 224, 255};

    SDL_Surface* surface_pause_message = TTF_RenderText_Solid(font, "PAUSED", grey);
    SDL_Texture* pause_message = SDL_CreateTextureFromSurface(renderer, surface_pause_message);
    float pause_message_width = 0.4 * WINDOW_WIDTH;
    float pause_message_height = 0.8 * background_height;
    SDL_Rect pause_message_rect = {
        (int)(0.5 * WINDOW_WIDTH - 0.5 * pause_message_width),
        (int)(0.5 * WINDOW_HEIGHT - 0.6 * pause_message_height),
        (int)(pause_message_width),
        (int)(pause_message_height)
    };
    SDL_RenderCopy(renderer, pause_message, NULL, &pause_message_rect);

    SDL_Surface* surface_quit_message = TTF_RenderText_Solid(font, "Press Q to quit", grey);
    SDL_Texture* quit_message = SDL_CreateTextureFromSurface(renderer, surface_quit_message);
    float quit_message_width = 0.18 * WINDOW_WIDTH;
    float quit_message_height = 0.15 * background_height;
    SDL_Rect quit_message_rect = {
        (int)(0.5 * WINDOW_WIDTH - 0.5 * quit_message_width),
        (int)(0.5 * WINDOW_HEIGHT + 0.3 * pause_message_height),
        (int)(quit_message_width),
        (int)(quit_message_height)
    };
    SDL_RenderCopy(renderer, quit_message, NULL, &quit_message_rect);

    SDL_FreeSurface(surface_pause_message);
    SDL_FreeSurface(surface_quit_message);
    SDL_DestroyTexture(pause_message);
    SDL_DestroyTexture(quit_message);

    return;
}

void render_end_screen(void) {
    float background_height = WINDOW_HEIGHT * 0.3;
    SDL_Rect background_rect = {
        0,
        (int)(0.5 * WINDOW_HEIGHT - 0.5 * background_height),
        WINDOW_WIDTH,
        (int)(background_height)
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &background_rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color grey = {224, 224, 224, 255};

    SDL_Surface* surface_game_over = TTF_RenderText_Solid(font, "GAME OVER", grey);
    SDL_Texture* game_over = SDL_CreateTextureFromSurface(renderer, surface_game_over);
    float game_over_width = 0.5 * WINDOW_WIDTH;
    float game_over_height = 0.8 * background_height;
    SDL_Rect game_over_rect = {
        (int)(0.5 * WINDOW_WIDTH - 0.5 * game_over_width),
        (int)(0.5 * WINDOW_HEIGHT - 0.6 * game_over_height),
        (int)(game_over_width),
        (int)(game_over_height)
    };
    SDL_RenderCopy(renderer, game_over, NULL, &game_over_rect);

    SDL_Surface* surface_reset = TTF_RenderText_Solid(font, "Press Q to quit, SPACE to reset", grey);
    SDL_Texture* reset = SDL_CreateTextureFromSurface(renderer, surface_reset);
    float reset_width = 0.35 * WINDOW_WIDTH;
    float reset_height = 0.15 * background_height;
    SDL_Rect reset_rect = {
        (int)(0.5 * WINDOW_WIDTH - 0.5 * reset_width),
        (int)(0.5 * WINDOW_HEIGHT + 0.275 * game_over_height),
        (int)(reset_width),
        (int)(reset_height)
    };
    SDL_RenderCopy(renderer, reset, NULL, &reset_rect);

    SDL_FreeSurface(surface_game_over);
    SDL_FreeSurface(surface_reset);
    SDL_DestroyTexture(game_over);
    SDL_DestroyTexture(reset);

    return;
}