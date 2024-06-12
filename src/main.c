#include "./main.h"

bool game_is_running = false;
bool game_started = false;
bool game_is_paused = false;
bool game_over = false;
bool muted = false;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

unsigned int starting_time = 0;
unsigned int current_time = 0;
unsigned int relative_current_time = 0;
unsigned int last_insect_spawn_time = 0;
unsigned int last_mini_boss_spawn_time = 0;
unsigned int last_laser_time = 0;

lasers_t* lasers_head = NULL;
insects_t* insects_head  = NULL;
mini_bosses_t* mini_bosses_head = NULL;
mini_boss_lasers_t* mini_bosses_lasers_head = NULL;

int main(void) {
    initialize_window_dimensions();
    if (WINDOW_WIDTH <= 0 || WINDOW_HEIGHT <= 0) {
        printf("Failed to initialize window dimensions.\n");
        return EXIT_FAILURE;
    }
    game_is_running = initialize_window();
    if (!game_is_running) {
        fprintf(stderr, "Failed to initialize window.\n");
        return EXIT_FAILURE;
    }
    printf("Window initialized.\n");
    printf("Starting game...\n");
    if (!setup()) {
        fprintf(stderr, "Failed to setup game.\n");
        return EXIT_FAILURE;
    }
    printf("Setup completed.\nEntering game loop...\n");

    while(!game_started && game_is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    game_is_running = false;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_q)
                        game_is_running = false;
                    game_started = true;
                    break;
            }
        }
        render();
        SDL_Delay(100);
        current_time = SDL_GetTicks();
    }

    while(game_is_running) {
        process_input();
        if (!game_is_paused && !game_over) {
            update();
            render();
            current_time = SDL_GetTicks();
        }
        else if(game_is_paused && !game_over) {
            render();
            SDL_Delay(100);
        }
        while(game_over && game_is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    game_is_running = false;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_q)
                        game_is_running = false;
                    if(event.key.keysym.sym == SDLK_SPACE) {
                        game_over = false;
                        reset();
                    }
                    break;
            }
        }
        render();
        SDL_Delay(100);
        current_time = SDL_GetTicks();
        }
    }
    
    destroy_window();
    printf("Game terminated.\nBye bye! :)\n");

    return 0;
}