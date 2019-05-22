//
// Created by Nozomi on 2019-05-11.
//

#include "gradient.h"
#include "variables.h"
#include "myMath.h"
#include <cmath>
#include <iostream>

bool checkRangeInt3(int x, int y, int z, int side){
    if ((checkRangeInt(x, side)) && (checkRangeInt(y, side)) && (checkRangeInt(z, side))){
        return true;
    } else {
        return false;
    }
}

float gradx(float *around) {
    float a[4] = {0, 0, 0, 0}, x1, x2;
    int count = 0;
    x1 = around[0], x2 = around[1];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[2], x2 = around[3];
    if ((x1 != 0) && (x2 != 0)){
        a[1] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[4], x2 = around[5];
    if ((x1 != 0) && (x2 != 0)){
        a[2] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[6], x2 = around[7];
    if ((x1 != 0) && (x2 != 0)){
        a[3] = slope2(x1, x2);
        count += 1;
    }
//    std::cout << a[0] << " " << a[1] << " " << around[0] << " " << around[1] << " " << slope2(around[0], around[1]) << std::endl;
    if (count != 0) {
        return (a[0] + a[1] + a[2] + a[3]) / count;
    } else {
        return 0;
    }
}

float grady(float *around) {
    float a[4] = {0, 0, 0, 0}, x1, x2;
    int count = 0;
    x1 = around[0], x2 = around[2];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[1], x2 = around[3];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[4], x2 = around[6];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[5], x2 = around[7];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    if (count != 0) {
        return (a[0] + a[1] + a[2] + a[3]) / count;
    } else {
        return 0;
    }
}

float gradz(float *around) {
    float a[4] = {0, 0, 0, 0}, x1, x2;
    int count = 0;
    x1 = around[0], x2 = around[4];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[1], x2 = around[5];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[2], x2 = around[6];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[3], x2 = around[7];
    if ((x1 != 0) && (x2 != 0)){
        a[0] = slope2(x1, x2);
        count += 1;
    }
    if (count != 0) {
        return (a[0] + a[1] + a[2] + a[3]) / count;
    } else {
        return 0;
    }
}

void gradRefraction(float *fRefractivity){
    for (unsigned int k=0; k<WIDTH; ++k){
        for (unsigned int j=0; j<HEIGHT; ++j){
            for (unsigned int i=0; i<DEPTH; ++i) {
                float around[] = {0, 0, 0, 0, 0, 0, 0, 0};
                if (checkRangeInt3(i - 1, j - 1, k - 1, WIDTH)) {
                    around[0] = fRefractivity[((i - 1) + (j - 1) * WIDTH + (k - 1) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i, j - 1, k - 1, WIDTH)) {
                    around[1] = fRefractivity[((i) + (j - 1) * WIDTH + (k - 1) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i - 1, j, k - 1, WIDTH)) {
                    around[2] = fRefractivity[((i - 1) + (j) * WIDTH + (k - 1) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i, j, k - 1, WIDTH)) {
                    around[3] = fRefractivity[((i) + (j) * WIDTH + (k - 1) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i - 1, j - 1, k, WIDTH)) {
                    around[4] = fRefractivity[((i - 1) + (j - 1) * WIDTH + (k) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i, j - 1, k, WIDTH)) {
                    around[5] = fRefractivity[((i) + (j - 1) * WIDTH + (k) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i - 1, j, k, WIDTH)) {
                    around[6] = fRefractivity[((i - 1) + (j) * WIDTH + (k) * WIDTH * HEIGHT)];
                }
                if (checkRangeInt3(i, j, k, WIDTH)) {
                    around[7] = fRefractivity[((i) + (j) * WIDTH + (k) * WIDTH * HEIGHT)];
                }
                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = gradx(around);
                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = grady(around);
                grad[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = gradz(around);
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