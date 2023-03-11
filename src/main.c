#include "graphics.h"

int main(int argc, char** argv){
    (void) argc; (void) argv; 
    
    Ctx ctx = {0};
    init_context(&ctx, "Nevo");

    while (ctx.is_running){
        handle_events(&ctx);
        render(&ctx);
        update(&ctx);
        SDL_Delay(1000/120);
    }

    destroy(&ctx);

    return 0;
}