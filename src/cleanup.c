#include "./main.h"

void free_all_enemies_and_projectiles(void) {
    lasers_t* current_laser = lasers_head;
    lasers_t* prev_laser = NULL;

    while(current_laser) {
        lasers_t* laser_to_delete = current_laser;

        if(prev_laser == NULL)
            lasers_head = current_laser->next;
        else 
            prev_laser->next = current_laser->next;

        current_laser = current_laser->next;
        free(laser_to_delete);
    }

    insects_t* current_insect = insects_head;
    insects_t* prev_insect = NULL;

    while(current_insect) {
        insects_t* insect_to_delete = current_insect;

        if(prev_insect == NULL)
            insects_head = current_insect->next;
        else    
            prev_insect->next = current_insect->next;

        current_insect = current_insect->next;
        free(insect_to_delete);
    }

    mini_bosses_t* current_mini_boss = mini_bosses_head;
    mini_bosses_t* prev_mini_boss = NULL;

    while(current_mini_boss) {
        mini_bosses_t* mini_boss_to_delete = current_mini_boss;

        if(prev_mini_boss == NULL)
            mini_bosses_head = current_mini_boss->next;
        else    
            prev_mini_boss->next = current_mini_boss->next;

        current_mini_boss = current_mini_boss->next;
        free(mini_boss_to_delete);
    }

    mini_boss_lasers_t* current_mini_boss_laser = mini_bosses_lasers_head;
    mini_boss_lasers_t* prev_mini_boss_laser = NULL;

    while(current_mini_boss_laser) {
        mini_boss_lasers_t* mini_boss_laser_to_delete = current_mini_boss_laser;

        if(prev_mini_boss_laser == NULL)
            mini_bosses_lasers_head = current_mini_boss_laser->next;
        else    
            prev_mini_boss_laser->next = current_mini_boss_laser->next;

        current_mini_boss_laser = current_mini_boss_laser->next;
        free(mini_boss_laser_to_delete);
    }

    return;
}

void destroy_window(void) {
    free_all_enemies_and_projectiles();

    for (int i = 0; i < 15; ++i) {
        SDL_DestroyTexture(textures.purple_bug[i]);
        SDL_DestroyTexture(textures.red_bug[i]);
    }
    for(int i = 0; i < 13; i++)
        SDL_DestroyTexture(textures.bee[i]);

    SDL_DestroyTexture(textures.aircraft);
    SDL_DestroyTexture(textures.background);
    SDL_DestroyTexture(textures.laser);
    SDL_DestroyTexture(textures.red_heart);
    SDL_DestroyTexture(textures.yellow_heart);
    SDL_DestroyTexture(textures.mini_boss);
    SDL_DestroyTexture(textures.mini_boss_laser);

    TTF_CloseFont(font);

    Mix_FreeChunk(sound_effects.background_sounds);
    Mix_FreeChunk(sound_effects.insects);
    Mix_FreeChunk(sound_effects.player_laser);
    Mix_FreeChunk(sound_effects.mini_boss_laser);
    Mix_FreeChunk(sound_effects.player_hit_impact);
    Mix_FreeChunk(sound_effects.insect_hit_impact);
    Mix_FreeChunk(sound_effects.mini_boss_hit_impact);
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return;
}