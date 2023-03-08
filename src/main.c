#include "graphics.h"

int main(int argc, char** argv){
    (void) argc; (void) argv; 
    
    // Network nn = {0};
    // size_t layer_dim[] = {3, 2, 4, 2};
    // init_network(&nn, layer_dim, 4);

    // float xs[] = {2.0f, 4.0f, 1.0f};
    // feedforward(&nn, xs, 3);

    // printf("out:\n");
    // for (size_t i = 0; i < nn.output_size; ++i){
    //     printf("%g\n", nn.output[i]);
    // }

    // free_network(&nn);

    // for (size_t i = 0; i < nn.layer_count; ++i){
    //     size_t rows = nn.layers[i].mat_rows;
    //     size_t cols = nn.layers[i].mat_cols;
        
    //     printf("layer %u weights (%u, %u): [\n", i, rows, cols);
    //     for (size_t j = 0; j < rows; ++j){
    //         printf("    ");
    //         for (size_t k = 0; k < cols; ++k){
    //             printf("%.3g  ", nn.layers[i].mat_data[j*cols + k]);
    //         }
    //         printf("\n");
    //     }
    //     printf("]\n");
    // }
    
    Ctx ctx = {0};
    init_sdl2(&ctx, "Nevo", 800, 800);

    while (ctx.is_running){
        handle_events(&ctx);
        render(&ctx);
        update(&ctx);
        SDL_Delay(1000/60);
    }

    destroy(&ctx);

    return 0;
}