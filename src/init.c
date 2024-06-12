#include "./main.h"

int WINDOW_WIDTH = 0;
int WINDOW_HEIGHT = 0;

void initialize_window_dimensions() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        printf("SDL_GetCurrentDisplayMode Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }
    WINDOW_WIDTH = displayMode.w;
    WINDOW_HEIGHT = displayMode.h;

    SDL_Quit();
}

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
        );
    if(!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
        return false;
    }

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        printf("Failed to initialize SDL audio subsystem: %s\n", SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        return false;
    }
    Mix_MasterVolume((int)(MASTER_VOLUME / 10. * SDL_MIX_MAXVOLUME));

    return true;
}

bool setup(void) {
    srand(time(NULL));

    player.width = 0.12 * WINDOW_WIDTH;
    player.height = 0.08 * WINDOW_WIDTH;
    player.x = 0.05 * WINDOW_WIDTH;
    player.y = 0.5 * WINDOW_HEIGHT - player.height * 0.5;
    player.speed = PLAYER_SPEED;
    player.health = 10;
    player.base_health = 5;
    player.ammo = 10;
    player.max_ammo = 10;
    player.score = 0;
    player.best_score=0;

    for (int i = 0; i < 15; ++i) {
        char filename[256];
        sprintf(filename, "../assets/textures/Purple Bug Sprites/png/skeleton-animation_%02d.png", i);
        textures.purple_bug[i] = IMG_LoadTexture(renderer, filename);
        if(!textures.purple_bug[i]) {
            fprintf(stderr, "Loading purple bug's texture failed: %s\n", IMG_GetError());
            return false;
        }
    }

    for (int i = 0; i < 15; ++i) {
        char filename[256];
        sprintf(filename, "../assets/textures/Star Bug Game Asset Sprites/png/skeleton-animation_%02d.png", i);
        textures.red_bug[i] = IMG_LoadTexture(renderer, filename);
        if(!textures.red_bug[i]) {
            fprintf(stderr, "Loading red bug's texture failed: %s\n", IMG_GetError());
            return false;
        }
    }

    for (int i = 0; i < 13; ++i) {
        char filename[256];
        sprintf(filename, "../assets/textures/bee sprites/sprites/skeleton-animation_%02d.png", i);
        textures.bee[i] = IMG_LoadTexture(renderer, filename);
        if(!textures.bee[i]) {
            fprintf(stderr, "Loading bee's texture failed: %s\n", IMG_GetError());
            return false;
        }
    }

    textures.aircraft = IMG_LoadTexture(renderer, "../assets/textures/aircrafts/10 Aircrafts Game Asset-01.png");
    if(!textures.aircraft) {
        fprintf(stderr, "Loading player's texture failed: %s\n", IMG_GetError());
        return false;
    }

    textures.background = IMG_LoadTexture(renderer, "../assets/textures/background/vecteezy_space-game-background-neon-night-alien-landscape_16961935.jpg");
    if(!textures.background) {
        fprintf(stderr, "Loading background texture failed: %s\n", IMG_GetError());
        return false;
    }

    textures.laser = IMG_LoadTexture(renderer, "../assets/textures/lasers/19.png");
    if(!textures.laser) {
        fprintf(stderr, "Loading laser texture failed: %s\n", IMG_GetError());
        return false;
    }

    textures.red_heart = IMG_LoadTexture(renderer, "../assets/textures/hearts/red.png");
    if(!textures.red_heart) {
        fprintf(stderr, "Loading red heart texture failed: %s\n", IMG_GetError());
        return false;
    }

    textures.yellow_heart = IMG_LoadTexture(renderer, "../assets/textures/hearts/yellow.png");
    if(!textures.yellow_heart) {
        fprintf(stderr, "Loading yellow heart texture failed: %s\n", IMG_GetError());
        return false;
    }

    textures.mini_boss = IMG_LoadTexture(renderer, "../assets/textures/aircrafts/10 Aircrafts Game Asset-10.png");
    if(!textures.mini_boss) {
        fprintf(stderr, "Loading mini boss's texture failed: %s\n", IMG_GetError());
        return false;
    }

    textures.mini_boss_laser = IMG_LoadTexture(renderer, "../assets/textures/lasers/14.png");
    if(!textures.mini_boss_laser) {
        fprintf(stderr, "Loading mini boss's texture failed: %s\n", IMG_GetError());
        return false;
    }  

    font = TTF_OpenFont("../assets/fonts/OpenSans-Bold.ttf", 0.1 * WINDOW_WIDTH);
    if (!font) {
        fprintf(stderr, "Loading font failed: %s\n", TTF_GetError());
        return false;
    }

    sound_effects.background_sounds = Mix_LoadWAV("../assets/sound_effects/Retro Ambience 02.wav");
    if(!sound_effects.background_sounds) {
        fprintf(stderr, "Loading background sounds failed: %s\n", Mix_GetError());
        return false;
    }
    Mix_PlayChannel(-1, sound_effects.background_sounds, -1);

    sound_effects.insects = Mix_LoadWAV("../assets/sound_effects/Retro Fly 01.wav");
    if(!sound_effects.insects) {
        fprintf(stderr, "Loading insects' sound failed: %s\n", Mix_GetError());
        return false;
    }

    sound_effects.player_laser = Mix_LoadWAV("../assets/sound_effects/Retro Gun Laser SingleShot 01.wav");
    if(!sound_effects.player_laser) {
        fprintf(stderr, "Loading mini boss's laser sound failed: %s\n", Mix_GetError());
        return false;
    }

    sound_effects.player_hit_impact = Mix_LoadWAV("../assets/sound_effects/Retro Impact 20.wav");
    if(!sound_effects.player_hit_impact) {
        fprintf(stderr, "Loading player's hit impact sound failed: %s\n", Mix_GetError());
        return false;
    }

    sound_effects.insect_hit_impact = Mix_LoadWAV("../assets/sound_effects/Retro Impact Punch Hurt 01.wav");
    if(!sound_effects.insect_hit_impact) {
        fprintf(stderr, "Loading insects' hit impact sound failed: %s\n", Mix_GetError());
        return false;
    }

    sound_effects.mini_boss_laser = Mix_LoadWAV("../assets/sound_effects/Retro Weapon Laser 36.wav");
    if(!sound_effects.mini_boss_laser) {
        fprintf(stderr, "Loading player laser's sound failed: %s\n", Mix_GetError());
        return false;
    }
    
    sound_effects.mini_boss_hit_impact = Mix_LoadWAV("../assets/sound_effects/Retro Impact Punch 07.wav");
    if(!sound_effects.mini_boss_hit_impact) {
        fprintf(stderr, "Loading mini bosses' hit impact sound failed: %s\n", Mix_GetError());
        return false;
    }


    return true;
}

void reset(void) {
    free_all_enemies_and_projectiles();

    player.x = 0.05 * WINDOW_WIDTH;
    player.y = 0.5 * WINDOW_HEIGHT - player.height * 0.5;
    player.health = 10;
    player.base_health = 5;
    player.ammo = 10;

    if(player.score > player.best_score)
        player.best_score = player.score;
    player.score = 0;

    starting_time = SDL_GetTicks();
    last_insect_spawn_time = 0;
    last_laser_time = 0;

    return;
}

