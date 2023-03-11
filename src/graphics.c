#include <time.h>
#include "graphics.h"

static float rand_betweenf(float x1, float x2){
    return (float)(rand() / (float)RAND_MAX) * (x2 - x1) + x2;
}

static int rand_between(int x1, int x2){
    return rand() % (x2 + 1 - x1) + x1;
}

void init_context(Ctx* ctx, const char* tite){
    
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
        fprintf(stderr, "ERROR: initialisation error\n");
        SDL_Quit();
        exit(1);
    }

    if (!(ctx->win = SDL_CreateWindow(tite, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            WIDTH, HEIGHT, 0)))
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
    add_wall(ctx, (Wall){10, 10, 10, HEIGHT-10});
    add_wall(ctx, (Wall){10, 10, WIDTH-10, 10});
    add_wall(ctx, (Wall){10, HEIGHT-10, WIDTH-10, HEIGHT-10});
    add_wall(ctx, (Wall){WIDTH-10, 10, WIDTH-10, HEIGHT-10});

    ctx->agent_texture = fill_circle_texture(ctx->ren, AGENT_RADIUS, 0xffaa33ff);
    ctx->area = (SDL_Rect){
        .x = 40, .y = 600,
        .w = 80, .h = 60
    };
    // 5 inputs
    //     ray[5] len
    // 2 outputs 
    //     value heading [0f..2PI]
    //     value forward force [-1f..1f] 
    size_t layer_dim[] = {RAY_COUNT, 4, 6, 2};

    for (size_t i = 0; i < AGENT_COUNT; ++i){
        Agent* agent = ctx->agents + i;
        init_network(&agent->nn, layer_dim, sizeof(layer_dim)/sizeof(size_t));
        agent->is_dead = false;
        agent->fitness = 0.0f;
        agent->radius = AGENT_RADIUS;
        agent->x = rand_between(ctx->area.x, ctx->area.x + ctx->area.w);
        agent->y = rand_between(ctx->area.y, ctx->area.y + ctx->area.h);;
        agent->angle = rand_betweenf(0.0f, 2.0f * PI);

        size_t j = 0; 
        float half_fov = AGENT_FOV/2.0f;
        float fov_per_ray = AGENT_FOV/RAY_COUNT;
        for (float degree = -half_fov; degree <= half_fov && j < RAY_COUNT; degree += fov_per_ray, ++j){
            float rad = degree/360.0f * 2.0f * PI;
            agent->rays[j].angle = rad;
            agent->rays[j].len = RAY_LEN;
        }
    }

    ctx->num_generation = 1;
    ctx->is_running = true;
    ctx->edit_mode = true;
    ctx->temp_point_set = false;
    ctx->num_agents_alive = AGENT_COUNT;

    printf("Generation %u\n", ctx->num_generation);
}

void add_wall(Ctx* ctx, Wall new_wall){
    if (ctx->num_walls >= ctx->cap_walls){
        ctx->cap_walls = ctx->cap_walls == 0 ? 8 : ctx->cap_walls * 2;
        
        ctx->walls = realloc(ctx->walls, sizeof(Wall) * (ctx->cap_walls));
        if (!ctx->walls){
            fprintf(stderr, "ERROR: buy more ram!\n");
            exit(1);
        }
    }
    ctx->walls[ctx->num_walls++] = new_wall;
}

void handle_events(Ctx* ctx){
    for (SDL_Event e; SDL_PollEvent(&e);){
        switch (e.type){
            case SDL_QUIT: ctx->is_running = false; break;

            case SDL_KEYDOWN: {
                switch (e.key.keysym.sym){
                    case SDLK_e: ctx->edit_mode ^= true; break;
                    case SDLK_n: ctx->num_agents_alive = 0; break;
                    
                    default: break;
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                if (!ctx->edit_mode) return;

                if (ctx->temp_point_set){
                    // create new wall
                    Wall new_wall = (Wall){
                        .x1 = (float) ctx->tempx,
                        .y1 = (float) ctx->tempy,
                        .x2 = (float) e.button.x,
                        .y2 = (float) e.button.y
                    };
                    add_wall(ctx, new_wall);
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

    // render starting area
    SDL_SetRenderDrawColor(ctx->ren, 0, 200, 200, 255);
    SDL_RenderFillRect(ctx->ren, &ctx->area);

    // render walls
    SDL_SetRenderDrawColor(ctx->ren, 255, 100, 100, 255);
    for (size_t i = 0; i < ctx->num_walls; ++i){
        Wall wall = ctx->walls[i];
        SDL_RenderDrawLineF(ctx->ren, wall.x1, wall.y1, wall.x2, wall.y2);
    }

    // render agents
    SDL_SetRenderDrawColor(ctx->ren, 255, 255, 255, 255);
    for (size_t i = 0; i < AGENT_COUNT; ++i){
        Agent* agent = ctx->agents + i;
        float x = agent->x;
        float y = agent->y;
        
        // render rays
        for (size_t j = 0; j < RAY_COUNT; ++j){
            float rx = x + agent->rays[j].len * SDL_cosf(agent->rays[j].angle + agent->angle);
            float ry = y + agent->rays[j].len * SDL_sinf(agent->rays[j].angle + agent->angle);
            SDL_RenderDrawLineF(ctx->ren, x, y, rx, ry);
        }

        // render agent
        SDL_FRect r = (SDL_FRect){x-agent->radius/2, y-agent->radius/2, agent->radius, agent->radius};
        SDL_RenderCopyF(ctx->ren, ctx->agent_texture, NULL, &r);
    }

    // render edit mode
    ctx->edit_mode ? 
        SDL_SetRenderDrawColor(ctx->ren, 30, 255, 80, 255) : 
        SDL_SetRenderDrawColor(ctx->ren, 20, 120, 255, 255);

    SDL_RenderFillRect(ctx->ren, &(SDL_Rect){10, 10, 40, 40});

    SDL_RenderPresent(ctx->ren);
}

static float p_dist(float x, float y, float x1, float y1, float x2, float y2){
    float A = x - x1;
    float B = y - y1;
    float C = x2 - x1;
    float D = y2 - y1;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = -1;
    if (len_sq != 0) //in case of 0 length line
        param = dot / len_sq;

    float xx, yy;

    if (param < 0) {
        xx = x1;
        yy = y1;
    }
    else if (param > 1) {
        xx = x2;
        yy = y2;
    }
    else {
        xx = x1 + param * C;
        yy = y1 + param * D;
    }

    float dx = x - xx;
    float dy = y - yy;
    return sqrtf(dx * dx + dy * dy);
}

static bool line_line_intersection(
    float *px, float *py, 
    float x1, float y1, 
    float x2, float y2, 
    float x3, float y3, 
    float x4, float y4) 
{
    float denom = (x1-x2)*(y3-y4) - (y1-y2) * (x3 - x4);
    if (denom != 0) {
        float t = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4))/denom;
        float u = ((x1-x3)*(y1-y2) - (y1-y3)*(x1-x2))/denom;
        if (0 <= t && t <= 1 && 0 <= u && u <= 1){
            // intersection hit
            *px = x1 + t*(x2-x1);
            *py = y1 + t*(y2-y1);
            return true;
        }
    }
    return false;
}

bool check_wall_collisions(Agent* agent, Wall* walls, size_t num_walls){
    for (size_t j = 0; j < RAY_COUNT; ++j){

        float closest_len = RAY_LEN;
        for (size_t i = 0; i < num_walls; ++i){

            // intersection between ray and wall? 
            // yes, then set ray length
            // no, then set ray length to RAY_LEN 
            float px, py;
            float x1 = agent->x;
            float y1 = agent->y;
            float x2 = agent->x + RAY_LEN * SDL_cosf(agent->rays[j].angle + agent->angle);
            float y2 = agent->y + RAY_LEN * SDL_sinf(agent->rays[j].angle + agent->angle);

            float x3 = walls[i].x1;
            float y3 = walls[i].y1;
            float x4 = walls[i].x2;
            float y4 = walls[i].y2;
            if (line_line_intersection(
                    &px, &py, 
                    x1, y1, 
                    x2, y2, 
                    x3, y3, 
                    x4, y4))
            {  
                float len = SDL_sqrtf(SDL_powf(x1-px, 2) + SDL_powf(y1-py, 2));
                if (len < closest_len) closest_len = len;
            }
            
            
        }
        agent->rays[j].len = closest_len;
    }

    for (size_t i = 0; i < num_walls; ++i){
        float dist = p_dist(agent->x, agent->y, walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2);
                
        if (dist < 10.0f) {
            return true;
        }
    }

    return false;
}

int fitness_compar(const void* a, const void* b) {
    return ((Agent*)b)->fitness - ((Agent*)a)->fitness;
}

void update(Ctx* ctx){

    if (ctx->edit_mode){

    } else {
        if (ctx->num_agents_alive == 0){
            // generate new generation
            // 1. sort agents
            qsort(ctx->agents, AGENT_COUNT, sizeof(Agent), fitness_compar);

            // 2. pick 2 best
            Agent p1 = ctx->agents[0];
            Agent p2 = ctx->agents[1];

            // 3. cross-over
            for (size_t i = 0; i < AGENT_COUNT; ++i){
                Agent* agent = ctx->agents + i;
                for (size_t j = 0; j < agent->nn.layer_count; ++j){
                    Layer* layer = agent->nn.layers + j;
                    Layer* p1_layer = p1.nn.layers + j;
                    Layer* p2_layer = p2.nn.layers + j;

                    size_t rand_rows = rand() % layer->mat_rows;
                    size_t rand_cols = rand() % layer->mat_cols;
                    
                    for (size_t k = 0, m = rand_rows; k < rand_rows; ++k, ++m){
                        layer->b_data[k] = p1_layer->b_data[k];
                        layer->b_data[m] = p2_layer->b_data[m];
                        // 4. mutate bias
                        float mut_rand = ((float)rand() / (float)RAND_MAX); 
                        if (mut_rand <= 0.1f) {
                            size_t rand_index = rand() % layer->mat_rows;
                            layer->b_data[rand_index] = ((float)rand() / (float)RAND_MAX);
                        }
                    }

                    for (size_t k = 0, m = rand_rows * rand_cols; k < rand_rows * rand_cols; ++k, ++m){
                        layer->mat_data[k] = p1_layer->mat_data[k];
                        layer->mat_data[m] = p2_layer->mat_data[m];
                        // 4. mutate bias
                        float mut_rand = ((float)rand() / (float)RAND_MAX); 
                        if (mut_rand <= 0.1f) {
                            size_t rand_index = rand() % (layer->mat_rows * layer->mat_cols);
                            layer->mat_data[rand_index] = ((float)rand() / (float)RAND_MAX);
                        }
                    }
                }
                // 5. reset variables
                agent->is_dead = false;
                agent->fitness = 0.0f;
                agent->x = rand_between(ctx->area.x, ctx->area.x + ctx->area.w);
                agent->y = rand_between(ctx->area.y, ctx->area.y + ctx->area.h);;
                agent->angle = rand_betweenf(0.0f, 2.0f * PI);
            }
            ctx->num_agents_alive = AGENT_COUNT;
            printf("Generation %u\n", ++ctx->num_generation);
            return;
        }

        for (size_t i = 0; i < AGENT_COUNT; ++i){
            Agent* agent = ctx->agents + i;

            if (agent->is_dead) continue;
            
            if (check_wall_collisions(agent, ctx->walls, ctx->num_walls)){
                // wall hit
                agent->is_dead = true;
                agent->fitness -= 2;
                ctx->num_agents_alive--;
                continue;
            }

            float xs[RAY_COUNT];
            for (size_t j = 0; j < RAY_COUNT; ++j){
                xs[j] = agent->rays[j].len;
                agent->fitness += (agent->rays[j].len / (float)(RAY_LEN * RAY_COUNT));
            }

            feedforward(&agent->nn, xs, RAY_COUNT);

            float out_angle = agent->nn.output[0] * 2.0f * PI;
            float out_force = agent->nn.output[1] < 0.5f ? -1.0f : 1.0f; //(agent->nn.output[1] * 2.0f - 1.0f) * 2.0f;

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

    free(ctx->walls);
    SDL_DestroyTexture(ctx->agent_texture);
    SDL_DestroyRenderer(ctx->ren);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
}