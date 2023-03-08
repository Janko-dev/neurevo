#include <time.h>
#include "graphics.h"

void init_sdl2(Ctx* ctx, const char* tite, size_t width, size_t height){
    
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
        fprintf(stderr, "ERROR: initialisation error\n");
        SDL_Quit();
        exit(1);
    }

    if (!(ctx->win = SDL_CreateWindow(tite, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            width, height, 0)))
    {
        fprintf(stderr, "ERROR: window initialisation error\n");
        SDL_Quit();
        exit(1);
    }

    if (!(ctx->ren = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_PRESENTVSYNC)))
    {
        fprintf(stderr, "ERROR: window initialisation error\n");
        SDL_DestroyWindow(ctx->win);
        SDL_Quit();
        exit(1);
    }

    size_t layer_dim[] = {3, 2, 4};
    for (size_t i = 0; i < AGENT_COUNT; ++i){
        Agent* agent = ctx->agents + i;
        init_network(&agent->nn, layer_dim, 3);
        agent->is_dead = false;
        agent->fitness = 0.0f;
        agent->x = (float)(rand() % width);
        agent->y = (float)(rand() % height);
        agent->angle = ((float)rand() / (float)RAND_MAX) * 2 * PI;

        size_t j = 0; 
        float half_fov = AGENT_FOV/2.0f;
        float fov_per_ray = AGENT_FOV/RAY_COUNT;
        for (float degree = -half_fov; degree <= half_fov && j < RAY_COUNT; degree += fov_per_ray, ++j){
            float rad = degree/360.0f * 2.0f * PI;
            agent->rays[j].angle = rad;
            agent->rays[j].len = RAY_LEN;
        }
    }

    ctx->is_running = true;
}

void handle_events(Ctx* ctx){
    for (SDL_Event e; SDL_PollEvent(&e);){
        switch (e.type){
            case SDL_QUIT: ctx->is_running = false; break;
            default: break;
        }
    }
}

void render(Ctx* ctx){
    SDL_SetRenderDrawColor(ctx->ren, 0, 0, 0, 0);
    SDL_RenderClear(ctx->ren);

    for (size_t i = 0; i < AGENT_COUNT; ++i){
        Agent* agent = ctx->agents + i;
        float x = agent->x;
        float y = agent->y;
        
        SDL_SetRenderDrawColor(ctx->ren, 255, 255, 255, 255);
        for (size_t j = 0; j < RAY_COUNT; ++j){
            float rx = x + agent->rays[j].len * SDL_cosf(agent->rays[j].angle + agent->angle);
            float ry = y + agent->rays[j].len * SDL_sinf(agent->rays[j].angle + agent->angle);

            SDL_RenderDrawLineF(ctx->ren, x, y, rx, ry);
        }
    }

    SDL_RenderPresent(ctx->ren);
}

void update(Ctx* ctx){
    (void)ctx;
}

void destroy(Ctx* ctx){
    for (size_t i = 0; i < AGENT_COUNT; ++i){
        free_network(&ctx->agents[i].nn);
    }
    SDL_DestroyRenderer(ctx->ren);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
}