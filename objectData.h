//
// Created by Nozomi on 2019-05-11.
//

#ifndef REFRACTION_OPENCL_OBJECTDATA_H
#define REFRACTION_OPENCL_OBJECTDATA_H

// position, radius, surface color, reflectivity, transparency, refractivity, emission color
void inSphere(float *center, float radius, float *surfaceColor, float reflectivity, float opacity, float refractivity, float *emissionColor);
void inconstantSphere(float *center, float radius, float *surfaceColor, float reflectivity, float opacity, float maxRefractivity, float *emissionColor);
void inPlain (float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float refractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity);
void inRectangle(float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float refractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity);
void inconstantRectangle(float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float maxRefractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity);
//void makeDataset(void);

#endif //REFRACTION_OPENCL_OBJECTDATA_H
