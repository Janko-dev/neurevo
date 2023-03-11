#include "nn.h"

void init_network(Network* nn, size_t* layer_dims, size_t layer_count){ 

    nn->layer_count = layer_count-1;
    nn->layers = (Layer*)malloc(sizeof(Layer) * layer_count);

    for (size_t i = 0; i < layer_count-1; ++i){

        size_t rows = layer_dims[i+1];
        size_t cols = layer_dims[i];

        nn->layers[i].mat_cols = cols;
        nn->layers[i].mat_rows = rows;

        nn->layers[i].mat_data = (float*)malloc(sizeof(float) * rows * cols);
        nn->layers[i].b_data   = (float*)malloc(sizeof(float) * rows);

        for (size_t j = 0; j < rows*cols; ++j){
            nn->layers[i].mat_data[j] = ((float)rand() / (float)RAND_MAX) * 2 - 1;
        }

        for (size_t j = 0; j < rows; ++j){
            nn->layers[i].b_data[j] = ((float)rand() / (float)RAND_MAX) * 2 - 1;
        }
    }
}

void free_network(Network* nn){
    for (size_t i = 0; i < nn->layer_count; ++i){
        free(nn->layers[i].mat_data);
        free(nn->layers[i].b_data);
    }
    free(nn->layers);
}

float sigmoid(float x){
    return 1.0/(1.0 + exp(-x));
} 

void feedforward(Network* nn, float* xs, size_t xs_size){

    if (xs_size != nn->layers[0].mat_cols){
        fprintf(stderr, "input vector must have the same size as columns of weight matrix\n");
        exit(1);
    }

    float prev_out[0xff];

    for (size_t i = 0; i < xs_size; ++i)
        prev_out[i] = xs[i];

    for (size_t i = 0; i < nn->layer_count; ++i){
        size_t rows    = nn->layers[i].mat_rows;
        size_t cols    = nn->layers[i].mat_cols;
        float* weights = nn->layers[i].mat_data;
        float* bias    = nn->layers[i].b_data;

        // nn->output = (float*)calloc(rows, sizeof(float));
        nn->output_size = rows;

        for (size_t j = 0; j < rows; ++j){
            for (size_t k = 0; k < cols; ++k){
                nn->output[j] += weights[j*cols + k] * prev_out[k];
            }
        }

        for (size_t j = 0; j < rows; ++j){
            nn->output[j] = sigmoid(nn->output[j] + bias[j]);
        }

        for (size_t i = 0; i < nn->output_size; ++i){
            prev_out[i] = nn->output[i];
        }
    }

}