#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "nn.h"
#include "texture.h"

#define RAY_COUNT 5
#define RAY_LEN   50

#define AGENT_FOV 60.0f
#define AGENT_RADIUS 10
#define AGENT_COUNT 100

#define PI 3.14159265359f

typedef struct {
    float len;
    float angle;
} Ray;

typedef struct {
    float x, y;
    float angle;
    float radius;
    Ray rays[RAY_COUNT];
    Network nn;
    float fitness;
    bool is_dead;
} Agent;

typedef struct {
    float x1, y1;
    float x2, y2;
} Wall;

typedef struct {
    SDL_Window* win;
    SDL_Renderer* ren;
    
    Agent agents[AGENT_COUNT];
    SDL_Texture* agent_texture;
    
    Wall* walls;
    size_t num_walls;
    size_t cap_walls;
    bool temp_point_set;
    size_t tempx, tempy;

    size_t num_agents_alive;
    bool is_running;
    bool edit_mode;
} Ctx;

void init_sdl2(Ctx* ctx, const char* tite, size_t width, size_t height);
void handle_events(Ctx* ctx);
void render(Ctx* ctx);
void update(Ctx* ctx);
void destroy(Ctx* ctx);

#endif // _GRAPHICS_H