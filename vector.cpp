//
// Created by Nozomi on 2019-05-06.
//

#include "vector.h"
#include "variables.h"
#include <cmath>
#include <math.h>

void addVec(float *a, float *b, float *c){
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
}

void subVec(float *a, float *b, float *c){
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

void multiVec(float *a, float *b, float *c){
    c[0] = a[0] * b[0];
    c[1] = a[1] * b[1];
    c[2] = a[2] * b[2];
}

void divVec(float *a, float *b, float *c){
    c[0] = a[0] / b[0];
    c[1] = a[1] / b[1];
    c[2] = a[2] / b[2];
}

void multiFloat(float *a, float b, float *c){
    c[0] = a[0] * b;
    c[1] = a[1] * b;
    c[2] = a[2] * b;
}

void divFloat(float *a, float b, float *c){
    c[0] = a[0] / b;
    c[1] = a[1] / b;
    c[2] = a[2] / b;
}

void subVecInt(int *a, int *b, int *c){
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

void multiVecInt(int *a, int *b, int *c){
    c[0] = a[0] * b[0];
    c[1] = a[1] * b[1];
    c[2] = a[2] * b[2];
}

float vectorDistance (float* vec1, float *vec2){
    return sqrt(pow((vec1[0] - vec2[0]), 2) + pow((vec1[1] - vec2[1]), 2) + pow((vec1[2] - vec2[2]), 2));
}

float vectorDistanceInt (int* vec1, int *vec2){
    return sqrt(pow((vec1[0] - vec2[0]), 2) + pow((vec1[1] - vec2[1]), 2) + pow((vec1[2] - vec2[2]), 2));
}

void setVec3Float (float *v1, float *v2){
    v1[0] = v2[0];
    v1[1] = v2[1];
    v1[2] = v2[2];
}

void setVec3Int (int *v1, int *v2) {
    v1[0] = v2[0];
    v1[1] = v2[1];
    v1[2] = v2[2];
}

void absVec3(float *vec){
    vec[0] = std::abs(vec[0]);
    vec[1] = std::abs(vec[1]);
    vec[2] = std::abs(vec[2]);
}

float inpro(float *vec1, float *vec2){
    return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
}


float vecNormalize(float *vec, float *normalized){
    float square = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
    if (square < 0.001) {
        normalized[0] = 0;
        normalized[1] = 0;
        normalized[2] = 0;
    } else {
        normalized[0] = vec[0] / sqrt(square);
        normalized[1] = vec[1] / sqrt(square);
        normalized[2] = vec[2] / sqrt(square);
    }
}


float getVector(int coordinate, float *vec){
    float x, y, z;
    z = (coordinate / (WIDTH * WIDTH));
    y = (coordinate - z * WIDTH * WIDTH) / WIDTH;
    x = (coordinate - y * WIDTH  - z * WIDTH * WIDTH);
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}