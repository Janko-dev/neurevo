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

    ctx->walls = NULL;
    ctx->num_walls = 0;
    ctx->cap_walls = 0;

    ctx->agent_texture = fill_circle_texture(ctx->ren, AGENT_RADIUS, 0xffaa33ff);

    // 5 inputs
    //     ray len
    // 2 outputs 
    //     value heading [0f..2PI]
    //     value forward force [-1f..1f] 
    size_t layer_dim[] = {RAY_COUNT, 4, 2};
    for (size_t i = 0; i < AGENT_COUNT; ++i){
        Agent* agent = ctx->agents + i;
        init_network(&agent->nn, layer_dim, 3);
        agent->is_dead = false;
        agent->fitness = 0.0f;
        agent->radius = AGENT_RADIUS;
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
    ctx->edit_mode = true;
    ctx->temp_point_set = false;
    ctx->num_agents_alive = AGENT_COUNT;
}

void handle_events(Ctx* ctx){
    for (SDL_Event e; SDL_PollEvent(&e);){
        switch (e.type){
            case SDL_QUIT: ctx->is_running = false; break;

            case SDL_KEYDOWN: {
                switch (e.key.keysym.sym){
                    case SDLK_e: ctx->edit_mode ^= true; break;
                    default: break;
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                if (!ctx->edit_mode) return;

                if (ctx->temp_point_set){
                    // create new wall
                    if (ctx->num_walls >= ctx->cap_walls){
                        ctx->cap_walls = ctx->cap_walls == 0 ? 8 : ctx->cap_walls * 2;
                        ctx->walls = realloc(ctx->walls, sizeof(Wall) * ctx->cap_walls);
                        if (!ctx->walls){
                            fprintf(stderr, "ERROR: buy more ram\n");
                            exit(1);
                        }
                    }
                    ctx->walls[ctx->num_walls++] = (Wall){
                        .x1 = (float) ctx->tempx,
                        .y1 = (float) ctx->tempy,
                        .x2 = (float) e.button.x,
                        .y2 = (float) e.button.y
                    };
                    ctx->temp_point_set = false;

                } else {
                    ctx->tempx = e.button.x;
                    ctx->tempy = e.button.y;
                    ctx->temp_point_set = true;
                }

            } break;
            default: break;
        }
    }
}

void render(Ctx* ctx){
    SDL_SetRenderDrawColor(ctx->ren, 0, 0, 0, 0);
    SDL_RenderClear(ctx->ren);

    SDL_SetRenderDrawColor(ctx->ren, 255, 100, 100, 255);
    for (size_t i = 0; i < ctx->num_walls; ++i){
        Wall wall = ctx->walls[i];
        SDL_RenderDrawLineF(ctx->ren, wall.x1, wall.y1, wall.x2, wall.y2);
    }

    SDL_SetRenderDrawColor(ctx->ren, 255, 255, 255, 255);
    for (size_t i = 0; i < AGENT_COUNT; ++i){
        Agent* agent = ctx->agents + i;
        float x = agent->x;
        float y = agent->y;
        
        for (size_t j = 0; j < RAY_COUNT; ++j){
            float rx = x + agent->rays[j].len * SDL_cosf(agent->rays[j].angle + agent->angle);
            float ry = y + agent->rays[j].len * SDL_sinf(agent->rays[j].angle + agent->angle);

            SDL_RenderDrawLineF(ctx->ren, x, y, rx, ry);
        }

        SDL_FRect r = (SDL_FRect){x-agent->radius/2, y-agent->radius/2, agent->radius, agent->radius};
        SDL_RenderCopyF(ctx->ren, ctx->agent_texture, NULL, &r);
    }

    ctx->edit_mode ? 
        SDL_SetRenderDrawColor(ctx->ren, 30, 255, 80, 255) : 
        SDL_SetRenderDrawColor(ctx->ren, 20, 120, 255, 255);

    SDL_RenderFillRect(ctx->ren, &(SDL_Rect){10, 10, 40, 40});

    SDL_RenderPresent(ctx->ren);
}

void update(Ctx* ctx){

    if (ctx->edit_mode){

    } else {
        if (ctx->num_agents_alive == 0){
            // regenerate new generation
            return;
        }

        for (size_t i = 0; i < AGENT_COUNT; ++i){
            Agent* agent = ctx->agents + i;
            
            float xs[RAY_COUNT];
            for (size_t j = 0; j < RAY_COUNT; ++j){
                xs[j] = agent->rays[j].len;
            }

            feedforward(&agent->nn, xs, RAY_COUNT);

            float out_angle = agent->nn.output[0] * 2.0f * PI;
            float out_force = agent->nn.output[1] * 2.0f - 1.0f;

            agent->angle = out_angle;
            agent->x += out_force * SDL_cosf(out_angle);
            agent->y += out_force * SDL_sinf(out_angle);
        }
    }

}

void destroy(Ctx* ctx){
    for (size_t i = 0; i < AGENT_COUNT; ++i){
        free_network(&ctx->agents[i].nn);
    }
    SDL_DestroyTexture(ctx->agent_texture);
    SDL_DestroyRenderer(ctx->ren);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
}