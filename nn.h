#ifndef _NN_H
#define _NN_H

#include "la.h"

typedef struct {
    Matrix* weights;
    Vector* bias;
} Layer;

typedef struct {
    Vector* input;
    Layer* layers;
    size_t layer_count;
    Vector* output;
} Network;

void init_network(Network* nn, int input_dim, int* layer_dims, size_t layer_count, int output_dim);
void free_network(Network* nn);

Vector* feedforward(Network* nn, Vector* xs);

// x1 * w1 + x2 * w2 + x3 * w3
// layer { weights: mx(input dim), bias: mx1 }
#endif //_NN_H
