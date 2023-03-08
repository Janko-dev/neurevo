#ifndef _NN_H
#define _NN_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    float* data;
    size_t rows;
    size_t cols;
} Matrix;

typedef struct {
    float* data;
    size_t rows;
} Vector;

typedef struct {
    // Matrix* weights;
    // Vector* bias;
    float* mat_data;
    float* b_data;
    size_t mat_rows;
    size_t mat_cols;

} Layer;

typedef struct {
    Layer* layers;
    size_t layer_count;
    float* output;
    size_t output_size;
} Network;

void init_network(Network* nn, size_t* layer_dims, size_t layer_count);
void free_network(Network* nn);

void feedforward(Network* nn, float* xs, size_t xs_size);

// x1 * w1 + x2 * w2 + x3 * w3
// layer { weights: mx(input dim), bias: mx1 }
#endif //_NN_H
