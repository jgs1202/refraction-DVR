//
// Created by Nozomi on 2019-05-11.
//

#include "gradient.h"
#include "variables.h"
#include "myMath.h"
#include <cmath>


void gradRefraction(float *fRefractivity){
    for (unsigned int k=0; k<WIDTH; ++k){
        for (unsigned int j=0; j<HEIGHT; ++j){
            for (unsigned int i=0; i<DEPTH; ++i){
                float x1 = 0., x2 = 0., y1 = 0., y2 = 0., z1 = 0., z2 = 0.;
                bool xx1 = false, xx2 = false, yy1 = false, yy2 = false, zz1 = false, zz2 = false;
                float origin = fRefractivity[(i + j * WIDTH + k * WIDTH * HEIGHT)];
                if (i != 0) {
                    xx1 = true;
                    x1 = fRefractivity[(i - 1 + j * WIDTH + k * WIDTH * HEIGHT)];
                }
                if (i != WIDTH - 1) {
                    xx2 = true;
                    x2 = fRefractivity[(i + 1 + j * WIDTH + k * WIDTH * HEIGHT)];
                } if (j != 0){
                    yy1 = true;
                    y1 = fRefractivity[(i + (j - 1) * WIDTH + k * WIDTH * HEIGHT)];
                } if (j != HEIGHT - 1) {
                    yy2 = true;
                    y2 = fRefractivity[(i + (j + 1) * WIDTH + k * WIDTH * HEIGHT)];
                } if (k != 0){
                    zz1 = true;
                    z1 = fRefractivity[(i + j * WIDTH + (k - 1) * WIDTH * HEIGHT)];
                } if (k != DEPTH - 1) {
                    zz2 = true;
                    z2 = fRefractivity[(i + j * WIDTH + (k + 1) * WIDTH * HEIGHT)];
                }
//                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = (xx1)? slope2(x1, origin) : 0;
//                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = (yy1)? slope2(y1, origin) : 0;
//                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = (zz1)? slope2(z1, origin) : 0;
//
                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = (xx1 * xx2)? slope3(x1, origin, x2) : ((xx1)? slope2(x1, origin) : slope2(origin, x2));
                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = (yy1 * yy2)? slope3(y1, origin, y2) : ((yy1)? slope2(y1, origin) : slope2(origin, y2));
                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = (zz1 * zz2)? slope3(z1, origin, z2) : ((zz1)? slope2(z1, origin) : slope2(origin, z2));
            }
        }
    }
}


void gradSmooth (unsigned int kernelSz){
    float* newGrad = new float[FIELDSZ * 3];
    unsigned int count;
    int halfKernel = std::floor(kernelSz / 2);
    for (int x=0; x < WIDTH; ++x){
        for (int y=0; y < HEIGHT; ++y){
            for (int z=0; z < DEPTH; ++z){
                float c[] = {(float)x, (float)y, (float)z};
                newGrad[getCoordinate(c)] = 0;
                newGrad[getCoordinate(c) + 1] = 0;
                newGrad[getCoordinate(c) + 2] = 0;
                count = 0;
                for(int i=0; i < kernelSz; ++i){
                    for (int j=0; j < kernelSz; ++j){
                        for (int k=0; k < kernelSz; ++k){
                            if (x - halfKernel + i < 0){
                                continue;
                            } else if (x - halfKernel + i >= WIDTH) {
                                continue;
                            } else if (y - halfKernel + j < 0) {
                                continue;
                            } else if (y - halfKernel + j >= HEIGHT) {
                                continue;
                            } else if (z - halfKernel + k < 0) {
                                continue;
                            } else if (z - halfKernel + k >= DEPTH) {
                                continue;
                            } else {
                                float cc[] = {(float)(x - halfKernel + i), (float)(y - halfKernel + j), (float)(z - halfKernel + k)};
                                newGrad[getCoordinate(c)] += grad[getCoordinate(cc)];
                                newGrad[getCoordinate(c) + 1] += grad[getCoordinate(cc) + 1];
                                newGrad[getCoordinate(c) + 2] += grad[getCoordinate(cc) + 2];
                                count++;
                            }
                        }
                    }
                }
                newGrad[getCoordinate(c)] = newGrad[getCoordinate(c)] / count;
                newGrad[getCoordinate(c) + 1] = newGrad[getCoordinate(c) + 1] / count;
                newGrad[getCoordinate(c) + 2] = newGrad[getCoordinate(c) + 2] / count;
            }
        }
    }
    for (int position=0; position<WIDTH * HEIGHT * DEPTH * 3; ++position){
        grad[position] = newGrad[position];
    }
    delete newGrad;
}


void gradGauss3Smooth (void){
    float* newGrad = new float[WIDTH * HEIGHT * DEPTH * 3];
    float gauss[27] = {0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 4, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0};
    int halfKernel = 1;
    for (int x=0; x < WIDTH; ++x){
        for (int y=0; y < HEIGHT; ++y){
            for (int z=0; z < DEPTH; ++z){
                float c[] = {(float)x, (float)y, (float)z};
                newGrad[getCoordinate(c)] = 0;
                newGrad[getCoordinate(c) + 1] = 0;
                newGrad[getCoordinate(c) + 2] = 0;
                for(int i=0; i < 3; ++i){
                    for (int j=0; j < 3; ++j){
                        for (int k=0; k < 3; ++k){
                            if (x - halfKernel + i < 0){
                                continue;
                            } else if (x - halfKernel + i >= WIDTH) {
                                continue;
                            } else if (y - halfKernel + j < 0) {
                                continue;
                            } else if (y - halfKernel + j >= HEIGHT) {
                                continue;
                            } else if (z - halfKernel + k < 0) {
                                continue;
                            } else if (z - halfKernel + k >= DEPTH) {
                                continue;
                            } else {
                                float cc[] = {(float)(x - halfKernel + i), (float)(y - halfKernel + j), (float)(z - halfKernel + k)};
                                newGrad[getCoordinate(c)] += grad[getCoordinate(cc)] * gauss[i + j * 3 + k * 9] / 28;
                                newGrad[getCoordinate(c) + 1] += grad[getCoordinate(cc) + 1] * gauss[i + j * 3 + k * 9] / 28;
                                newGrad[getCoordinate(c) + 2] += grad[getCoordinate(cc) + 2] * gauss[i + j * 3 + k * 9] / 28;
//                                newGrad[getCoordinate(Vec3f(x, y, z))] += grad[getCoordinate(Vec3f(x - halfKernel + i, y - halfKernel + j, z - halfKernel + k))] * gauss[i + j * 3 + k * 9] / 28;
                            }
                        }
                    }
                }
            }
        }
    }
    for (int position=0; position<WIDTH * HEIGHT * DEPTH * 3; ++position){
        grad[position] = newGrad[position];
    }
    delete newGrad;
}

void gradGauss3SmoothWithCondition (void){
    float* newGrad = new float[WIDTH * HEIGHT * DEPTH * 3];
    float checkValue = 0; //std::numeric_limits<float>::infinity();
    float gauss[27] = {0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 4, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0};
    int halfKernel = 1;
    for (int x=0; x < WIDTH; ++x){
        for (int y=0; y < HEIGHT; ++y){
            for (int z=0; z < DEPTH; ++z){
                float c[] = {(float)x, (float)y, (float)z};
                bool flag = false, flagBegin = false;
                float old;
                newGrad[getCoordinate(c)] = 0;
                newGrad[getCoordinate(c) + 1] = 0;
                newGrad[getCoordinate(c) + 2] = 0;
                for(int i=0; i < 3; ++i){
                    for (int j=0; j < 3; ++j){
                        for (int k=0; k < 3; ++k){
                            if (x - halfKernel + i < 0){
                                continue;
                            } else if (x - halfKernel + i >= WIDTH) {
                                continue;
                            } else if (y - halfKernel + j < 0) {
                                continue;
                            } else if (y - halfKernel + j >= HEIGHT) {
                                continue;
                            } else if (z - halfKernel + k < 0) {
                                continue;
                            } else if (z - halfKernel + k >= DEPTH) {
                                continue;
                            } else {
                                float cc[] = {(float)(x - halfKernel + i), (float)(y - halfKernel + j), (float)(z - halfKernel + k)};
                                float refraction = fRefractivity[getScalarCoo(cc)];
                                if (!flagBegin) {
                                    flagBegin = true;
                                    old = refraction;
                                }
                                if (old != refraction) {
                                    flag = true;
                                }
                                newGrad[getCoordinate(c)] += grad[getCoordinate(cc)] * gauss[i + j * 3 + k * 9] / 28;
                                newGrad[getCoordinate(c) + 1] += grad[getCoordinate(cc) + 1] * gauss[i + j * 3 + k * 9] / 28;
                                newGrad[getCoordinate(c) + 2] += grad[getCoordinate(cc) + 2] * gauss[i + j * 3 + k * 9] / 28;
//                                newGrad[getCoordinate(Vec3f(x, y, z))] += grad[getCoordinate(Vec3f(x - halfKernel + i, y - halfKernel + j, z - halfKernel + k))] * gauss[i + j * 3 + k * 9] / 28;
                            }
                        }
                    }
                }
                if (!flag) {
                    newGrad[getCoordinate(c)] = checkValue;
                    newGrad[getCoordinate(c + 1)] = checkValue;
                    newGrad[getCoordinate(c + 2)] = checkValue;
                }
            }
        }
    }
    for (int position=0; position<WIDTH * HEIGHT * DEPTH * 3; ++position){
        if (newGrad[position] != checkValue){
            grad[position] = newGrad[position];
        }
    }
    delete newGrad;
}