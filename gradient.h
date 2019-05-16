//
// Created by Nozomi on 2019-05-11.
//

#ifndef REFRACTION_OPENCL_GRADIENT_H
#define REFRACTION_OPENCL_GRADIENT_H

void gradRefraction(float *fRefractivity);
void gradSmooth (unsigned int kernelSz);
void gradGauss3Smooth (void);
void gradGauss3SmoothWithCondition (void);

#endif //REFRACTION_OPENCL_GRADIENT_H