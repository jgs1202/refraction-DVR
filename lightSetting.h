//
// Created by Nozomi on 2019-07-06.
//

#ifndef REFRACTION_OPENCL_LIGHTSETTING_H
#define REFRACTION_OPENCL_LIGHTSETTING_H


void visPhotons(float* photonDirection, int photonNumber);
void visDirections(int photonNumber);
void visIntensity(int photonNumber);
void arrangePhotonEqually(float* photonPosition, float* photonDirection, float* position, float* color, int intensity, int photonNumber);
void insideLight(float *position, float *color, float intensity);
void triGrad(float x, float y, float z, float *newGrad, int side);
void photonParticipate(int photonNumber);
void gpuParticipate(int photonNumber);
void setLight1(void);

#endif //REFRACTION_OPENCL_LIGHTSETTING_H
