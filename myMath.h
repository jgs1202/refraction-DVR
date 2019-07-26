//
// Created by Nozomi on 2019-05-06.
//

#ifndef REFRACTION_OPENCL_MYMATH_H
#define REFRACTION_OPENCL_MYMATH_H

float roundInt (float x);
float slope2 (float p1, float p2);
float slope3 (float p1, float p2, float p3);
bool checkInteger(float *vec);
unsigned long getCoordinate(float *coordinate);
unsigned long getCoordinateInt(int *coordinate);
unsigned long getScalarCoo(float *coordinate);
void checkMinus(float *vec);
bool checkRangeInt(int x, int side);
float checkRange(float x, int side);
float gaussFunc(float x);
float exponential(float x);
bool checkPosition(int x, int y, int z, int side);

#endif //REFRACTION_OPENCL_MYMATH_H
