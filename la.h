#ifndef _LA_
#define _LA_

typedef struct{
    float** data;
    int m;
    int n;
} Matrix;

typedef struct{
    float* data;
    int n;
} Vector;

// Create
Matrix* create_mat(int m, int n);
Vector* create_vec(int n);
// Destroy
void free_mat(Matrix* matrix);
void free_vec(Vector* vector);
// Print 
void print_mat(Matrix* matrix);
void print_vec(Vector* vector);
// Product
Vector* prod_mat_vec(Matrix* mat, Vector* vec);
Matrix* prod_mat(Matrix* A, Matrix* B);
// Scale 
void scale_vec(Vector* vec, float scalar);
void scale_mat(Matrix* mat, float scalar);
// Apply function to members
void apply_vec(Vector* vec, float(*func)(float));
void apply_mat(Matrix* vec, float(*func)(float));
// Convert 
Matrix* vec_to_mat(Vector* vec);
Vector* mat_to_vec(Matrix* mat);
// Randomize values of matrix between min and max 
void randomize(Matrix* mat, float min, float max);
// Transpose Matrix
void transpose(Matrix* mat);
// Hard copy 
Matrix* copy_mat(Matrix* src); 
Vector* copy_vec(Vector* src); 
// Cross product of vectors
Vector* cross_prod(Vector* a, Vector* b);
// Gaussian elimination of matrix and vector to get row echelon form
// x * * | v1
// 0 x * | v2
// 0 0 x | v3
Matrix* gaussian_row_echelon(Matrix* mat, Vector* vec);

#endif