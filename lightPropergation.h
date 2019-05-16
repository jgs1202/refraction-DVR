//
// Created by Nozomi on 2019-05-11.
//

#ifndef REFRACTION_OPENCL_LIGHTPROPERGATION_H
#define REFRACTION_OPENCL_LIGHTPROPERGATION_H


int momGauss(int x, int y, int dx, int dy, int side);
int momMean(int x, int y, int dx, int dy, int side);
void lightSmoothing(float *plainData);
void gpuCalcLightSource (float *direction, float *color);

#endif //REFRACTION_OPENCL_LIGHTPROPERGATION_H
