//
// Created by Nozomi on 2019-05-11.
//

#ifndef REFRACTION_OPENCL_VARIABLES_H
#define REFRACTION_OPENCL_VARIABLES_H

extern const int WIDTH;
extern const int HEIGHT;
extern const int DEPTH;
extern const int WIDTH;
extern const int HEIGHT;
extern const int DEPTH;
extern const int STEP;
extern const int MAX_SOURCE_SIZE;
extern const int FIELDSZ;
extern const int PLAINSZ;
extern const int AOIx;
extern const int AOIy;
extern int animationCount;
extern bool updateState;
extern float shiftX;
extern float shiftY;

extern float* grad;
extern float* fColor;
extern float* fMedium;
extern float* fOpacity;
extern float* fRefractivity;

extern float* lColor;
extern float* lDirection;
extern float* lIntensity;

extern int viewW;
extern int viewH;

extern int PLATFORM_MAX;
extern int DEVICE_MAX;

extern int plainY;
extern float* castPositionY;
extern int castIndexY;

extern int plainX;
extern float* castPositionX;
extern int castIndexX;

extern int plainZ;

extern float* photonIrradiance;
extern int maxPhotons;
extern float* photonPosition;
extern float* photonDirection;
extern float* photonGradDirection;
extern float* photonColor;
extern int *photonStartTime;
extern int photonEnd;

//extern std::vector <float> surfaces;

#endif //REFRACTION_OPENCL_VARIABLES_H
