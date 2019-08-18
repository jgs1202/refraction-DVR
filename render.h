//
// Created by Nozomi on 2019-08-11.
//

#ifndef REFRACTION_OPENCL_RENDER_H
#define REFRACTION_OPENCL_RENDER_H

void bilinearTrace(float xx, float yy, unsigned int z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y);
void trace(float *angle, float x, float y, float *pixel);
void trilinearTrace(float xx, float yy, float z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y);
void trace3d(float *angle, float x, float y, float z, float *pixel);
void renderlColor(float distance, float *pixel);
void renderlColorRotate(float distance, float *pixel);
void gpuRender(float distance, float *pixel, float* rayPositions, float* rayAngles);
void gpuRenderlColor(float distance, float *pixel, float* rayPositions, float* rayAngles);

#endif //REFRACTION_OPENCL_RENDER_H
