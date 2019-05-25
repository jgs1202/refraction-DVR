//
// Created by Nozomi on 2019-05-06.
//

#ifndef REFRACTION_OPENCL_MYMATH_H
#define REFRACTION_OPENCL_MYMATH_H

float roundInt (float x);
float slope2 (float p1, float p2);
float slope3 (float p1, float p2, float p3);
bool checkInteger(float *vec);
unsigned int getCoordinate(float *coordinate);
unsigned int getScalarCoo(float *coordinate);
void checkMinus(float *vec);
bool checkRangeInt(int x, int side);
float checkRange(float x, int side);
float gaussFunc(float x);

#endif //REFRACTION_OPENCL_MYMATH_H
