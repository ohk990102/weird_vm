#include <stdlib.h>
#include "vector.h"

vector * new_vector(int capacity) {
    if(capacity <= 0)
        return NULL;

    vector * ret = (vector *) malloc(sizeof(vector));
    ret->capacity = capacity;
    ret->size = 0;
    ret->data = (int *) malloc(sizeof(int) * capacity);

    return ret;
}
void add_vector(vector * v, int n) {
    if(v->capacity == v->size)
        return;
    v->data[v->size++] = n;
}
void delete_vector(vector * v, int idx) {
    if(idx < 0 || idx >= v->capacity)
        return;
    v->data[idx] = 0;

    for(int i = idx; i < v->capacity - 1; i++) {
        v->data[i] = v->data[i+1];
        v->data[i+1] = 0;
    }
    v->size--;

}