//
// Created by Nozomi on 2019-05-06.
//

#ifndef REFRACTION_OPENCL_GPUSETTING_H
#define REFRACTION_OPENCL_GPUSETTING_H

#include <OpenCL/opencl.h>

extern cl_int err;

void EC(cl_int result, const char *title);
void EC2(const char *title);

#endif //REFRACTION_OPENCL_GPUSETTING_H
