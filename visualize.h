//
// Created by Nozomi on 2019-07-25.
//

#ifndef REFRACTION_OPENCL_VISUALIZE_H
#define REFRACTION_OPENCL_VISUALIZE_H

#include <opencv2/opencv.hpp>

void cvArrow(CvArr* img, CvPoint pt1, CvPoint pt2, CvScalar color, int thickness=1, int lineType=8, int shift=0);

#endif //REFRACTION_OPENCL_VISUALIZE_H
