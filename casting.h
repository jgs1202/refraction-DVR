//
// Created by Nozomi on 2019-05-11.
//

#ifndef REFRACTION_OPENCL_CASTING_H
#define REFRACTION_OPENCL_CASTING_H

void bilinearTrace(float xx, float yy, unsigned int z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y);
void trace(float *angle, float x, float y, float *pixel);
void trilinearTrace(float xx, float yy, unsigned int z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y);
void trace3d(float *angle, float x, float y, float *pixel);
void render(float distance, float *pixel);
void gpuRender(float distance, float *pixel);

#endif //REFRACTION_OPENCL_CASTING_H
