#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "nn.h"
#include "texture.h"

#define WIDTH 800
#define HEIGHT 800

#define RAY_COUNT 5
#define RAY_LEN   50

#define AGENT_FOV 60.0f
#define AGENT_RADIUS 10
#define AGENT_COUNT 100

#define PI 3.14159265359f

#define MAX(x, y) ((x) < (y) ? y : x)
#define MIN(x, y) ((x) < (y) ? x : y)

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

    SDL_Rect area;
    
    Agent agents[AGENT_COUNT];
    SDL_Texture* agent_texture;
    
    Wall* walls;
    size_t num_walls;
    size_t cap_walls;
    bool temp_point_set;
    size_t tempx, tempy;

    size_t num_agents_alive;
    size_t num_generation;
    bool is_running;
    bool edit_mode;
} Ctx;

void init_context(Ctx* ctx, const char* tite);
void handle_events(Ctx* ctx);
void render(Ctx* ctx);
void update(Ctx* ctx);
void destroy(Ctx* ctx);

void add_wall(Ctx* ctx, Wall new_wall);

#endif // _GRAPHICS_H