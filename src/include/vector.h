#ifndef VECTOR_H
#define VECTOR_H

typedef struct vector {
    int size;
    int capacity;
    int * data;
} vector;

vector * new_vector(int capacity);
void add_vector(vector * v, int n);
void delete_vector(vector * v, int idx);

#endif