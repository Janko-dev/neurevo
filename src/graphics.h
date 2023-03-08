#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "nn.h"

#define RAY_COUNT 5
#define RAY_LEN   100

#define AGENT_FOV 60.0f
#define AGENT_COUNT 100

#define PI 3.14159265359f

typedef struct {
    float len;
    float angle;
} Ray;

typedef struct {
    float x, y;
    float angle;
    Ray rays[RAY_COUNT];
    Network nn;
    float fitness;
    bool is_dead;
} Agent;

typedef struct {
    SDL_Window* win;
    SDL_Renderer* ren;
    Agent agents[AGENT_COUNT];
    bool is_running;
} Ctx;

void init_sdl2(Ctx* ctx, const char* tite, size_t width, size_t height);
void handle_events(Ctx* ctx);
void render(Ctx* ctx);
void update(Ctx* ctx);
void destroy(Ctx* ctx);

#endif // _GRAPHICS_H