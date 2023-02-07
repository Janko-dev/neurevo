#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "la.h"

// create
Matrix* create_mat(int m, int n){
    Matrix* result = (Matrix*)malloc(sizeof(Matrix));
    result->m = m;
    result->n = n;
    result->data = (float**)malloc(sizeof(float*)*result->m);
    for (int i = 0; i < result->m; i++){
        result->data[i] = (float*)calloc(result->n, sizeof(float));
    }
    return result;
}

Vector* create_vec(int n){
    Vector* result = (Vector*)malloc(sizeof(Vector));
    result->n = n;
    result->data = (float*)calloc(result->n, sizeof(float));
    return result;
}

// destroy
void free_mat(Matrix* matrix){
    for (int i = 0; i < matrix->m; i++){
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

void free_vec(Vector* vector){
    free(vector->data);
    free(vector);
}

// debug/print
void print_mat(Matrix* matrix){
    for (int i = 0; i < matrix->m; i++){
        for (int j = 0; j < matrix->n; j++){
            printf("%9.4f ", matrix->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_vec(Vector* vector){
    for (int i = 0; i < vector->n; i++){
        printf("%9.4f\n", vector->data[i]);
    }
    printf("\n");
}

// matrix/vector product
Vector* prod_mat_vec(Matrix* mat, Vector* vec){
    if (vec->n != mat->n){
        fprintf(stderr, "ERROR: Columns of matrix do not match rows of vector\n");
        exit(1);
    }

    Vector* result = create_vec(mat->m);
    for (int i = 0; i < mat->m; i++){
        for (int j = 0; j < mat->n; j++){
            result->data[i] += mat->data[i][j] * vec->data[j];
        }
    }
    return result;
}

Matrix* prod_mat(Matrix* A, Matrix* B){
    if (B->m != A->n){
        fprintf(stderr, "ERROR: Columns of matrix A do not match rows of matrix B\n");
        exit(1);
    }

    Matrix* result = create_mat(A->m, B->n);
    for (int i = 0; i < A->m; i++){
        for (int j = 0; j < B->n; j++){
            for (int k = 0; k < A->n; k++){
                result->data[i][j] += A->data[i][k] * B->data[k][j];    
            }
        }
    }
    return result;
}

// scale matrix/vector
void scale_vec(Vector* vec, float scalar){
    for (int i = 0; i < vec->n; i++) vec->data[i] *= scalar;
}

void scale_mat(Matrix* mat, float scalar){
    for (int i = 0; i < mat->m; i++) {
        for (int j = 0; j < mat->n; j++) {
            mat->data[i][j] *= scalar;
        }
    }
}

// apply function to matrix/vector
void apply_vec(Vector* vec, float(*func)(float)){
    for (int i = 0; i < vec->n; i++) vec->data[i] = func(vec->data[i]);
}

void apply_mat(Matrix* mat, float(*func)(float)){
    for (int i = 0; i < mat->m; i++) {
        for (int j = 0; j < mat->n; j++) {
            mat->data[i][j] = func(mat->data[i][j]);
        }
    }
}

// Convert vector to matrix and matrix to vector
Matrix* vec_to_mat(Vector* vec){
    Matrix* res = create_mat(vec->n, 1);
    for (int i = 0; i < res->m; i++){
        res->data[i][0] = vec->data[i];
    }
    return res;
}

Vector* mat_to_vec(Matrix* mat){
    if (mat->n != 1){
        fprintf(stderr, "ERROR: Columns of matrix must be 1 to convert to vector\n");
        exit(1);
    }
    Vector* res = create_vec(mat->m);
    for (int i = 0; i < res->n; i++){
        res->data[i] = mat->data[i][0];
    }
    return res;
}

// Randomize matrix
void randomize(Matrix* mat, float min, float max){
    if (min > max){
        fprintf(stderr, "ERROR: min %f must be smaller than max %f\n", min, max);
        exit(1);
    }
    for (int i = 0; i < mat->m; i++) {
        for (int j = 0; j < mat->n; j++) {
            mat->data[i][j] = (max-min) * ((float)rand()/(float)RAND_MAX) + min;
        }
    }
}

// Transpose matrix
void transpose(Matrix* mat){
    Matrix* res = create_mat(mat->n, mat->m);
    for (int i = 0; i < res->m; i++){
        for (int j = 0; j < res->n; j++){
            res->data[i][j] = mat->data[j][i];
        }    
    }
    free_mat(mat);
    mat = res;
}

Matrix* copy_mat(Matrix* src){
    Matrix* res = create_mat(src->m, src->n);
    for (int i = 0; i < src->m; i++){
        for (int j = 0; j < src->n; j++){
            res->data[i][j] = src->data[i][j];
        }
    }
    return res;
}

Vector* copy_vec(Vector* vec){
    Vector* res = create_vec(vec->n);
    for (int i = 0; i < vec->n; i++) res->data[i] = vec->data[i];
    return res;
}

Vector* cross_prod(Vector* a, Vector* b){
    if (a->n != 3 || b->n != 3){
        fprintf(stderr, "ERROR: vectors must be of length 3\n");
        exit(1);
    }
    Vector* res = create_vec(3);
    res->data[0] = a->data[1] * b->data[2] - a->data[2] * b->data[1];
    res->data[1] = a->data[2] * b->data[0] - a->data[0] * b->data[2];
    res->data[2] = a->data[0] * b->data[1] - a->data[1] * b->data[0];
    return res;
}

// Gaussian elimination of matrix and vector to get row echelon form
Matrix* gaussian_row_echelon(Matrix* mat, Vector* vec){
    if (vec->n != mat->m){
        fprintf(stderr, "ERROR: lenth of vector should equal rows of matrix\n");
        exit(1);
    }

    Matrix* res = create_mat(mat->m, mat->n+1);
    for (int i = 0; i < mat->m; i++){
        for (int j = 0; j < mat->n; j++){
            res->data[i][j] = mat->data[i][j];
        }   
    }
    for (int i = 0; i < vec->n; i++) res->data[i][res->n-1] = vec->data[i];
    print_mat(res);

    for (int k = 0; k < res->n; k++){
        for (int i = k+1; i < res->m; i++){
            float val = res->data[i][k]/res->data[k][k];
            for (int j = k; j < res->n; j++){
                res->data[i][j] -= res->data[k][j] * val;
            }
        }
    }
    return res;
}
