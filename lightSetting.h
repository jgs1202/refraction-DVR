//
// Created by Nozomi on 2019-07-06.
//

#ifndef REFRACTION_OPENCL_LIGHTSETTING_H
#define REFRACTION_OPENCL_LIGHTSETTING_H


void visPhotons(float* photonDirection, int photonNumber);
void arrangePhotonEqually(float* photonPosition, float* photonDirection, float* position, float* color, int intensity, int photonNumber);
void insideLight(float *position, float *color, float intensity);

#endif //REFRACTION_OPENCL_LIGHTSETTING_H
