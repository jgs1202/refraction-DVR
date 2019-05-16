//
// Created by Nozomi on 2019-05-06.
//

#ifndef REFRACTION_OPENCL_VECTOR_H
#define REFRACTION_OPENCL_VECTOR_H

void addVec(float *a, float *b, float *c);
void subVec(float *a, float *b, float *c);
void multiVec(float *a, float *b, float *c);
void divVec(float *a, float *b, float *c);
void multiFloat(float *a, float b, float *c);
void divFloat(float *a, float b, float *c);
void subVecInt(int *a, int *b, int *c);
void multiVecInt(int *a, int *b, int *c);
float vectorDistance (float* vec1, float *vec2);
float vectorDistanceInt (int* vec1, int *vec2);
void setVec3Float (float *v1, float *v2);
void setVec3Int (int *v1, int *v2);
void absVec3(float *vec);
float inpro(float *vec1, float *vec2);
float vecNormalize(float *vec, float *normalized);
float getVector(int coordinate, float *vec);


#endif //REFRACTION_OPENCL_VECTOR_H
