//
// Created by Nozomi on 2019-05-06.
//

#include <OpenCL/opencl.h>
#include <iostream>

cl_int err = CL_SUCCESS;

void EC(cl_int result, const char *title)
{
    if (result != CL_SUCCESS) {
        std::cout << "Error: " << title << "(" << result << ")\n";
    }
}
void EC2(const char *title)
{
    if (err != CL_SUCCESS) {
        std::cout << "Error: " << title << "(" << err << ")\n";
    }
    err = CL_SUCCESS;
}