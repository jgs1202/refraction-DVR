//
// Created by Nozomi on 2019-05-06.
//


#include "variables.h"
#include "myMath.h"
#include <iostream>
#include <cmath>

float roundInt (float x){
    float decimal = x - std::floor(x);
    if (decimal < 0.5) {
        return std::floor(x);
    } else {
        return std::ceil(x);
    }
}

float slope2 (float p1, float p2){
    return (p2 - p1) / (100 / (float)WIDTH);
}
float slope3 (float p1, float p2, float p3){
    return ((p2 - p1) / 2 + (p3 - p2) / 2) / (100 / (float)WIDTH);
}

bool checkInteger(float *vec){
    if ((std::ceil(vec[0]) == std::floor(vec[0])) && (std::ceil(vec[1]) == std::floor(vec[1])) && (std::ceil(vec[2]) == std::floor(vec[2])) ){
        return true;
    } else {
        return false;
    }
}

unsigned long getCoordinate(float *coordinate){
    return ((int)coordinate[0] + (int)coordinate[1] * WIDTH + (int)coordinate[2] * WIDTH * WIDTH) * 3;
}

unsigned long getCoordinateInt(int *coordinate){
    return (coordinate[0] + coordinate[1] * WIDTH + coordinate[2] * WIDTH * WIDTH);
}

unsigned long getScalarCoo(float *coordinate){
    return ((int)coordinate[0] + (int)coordinate[1] * WIDTH + (int)coordinate[2] * WIDTH * WIDTH);
}

void checkMinus(float *vec){
    if (vec[0] < 0){
        vec[0] = 0;
    } if (vec[1] < 0) {
        vec[1] = 0;
    } if (vec[2] < 0) {
        vec[2] = 0;
    }
}

bool checkRangeInt(int x, int side){
    if ((x >= 0) && (x <= side - 1)){
        return true;
    } else {
        return false;
    }
}

bool checkPosition(int x, int y, int z, int side) {
    if ((x < 0) || (x > side - 1)){
        return false;
    } else if ((y < 0) || (y > side - 1)) {
        return false;
    } else if ((z < 0) || (z > side - 1)) {
        return false;
    } else {
        return true;
    }
}

float checkRange(float x, int side){
    float c = x;
    if (x < 0){
        c = 0;
    } else if (x > side - 1){
        c = side - 1;
    }
    return c;
}

float gaussFunc(float x) {
    float c = 50;
    return exp(-x * x / 2 / c / c);
}

float exponential(float x) {
    return exp(x);
}