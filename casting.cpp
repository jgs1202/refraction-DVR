//
// Created by Nozomi on 2019-05-11.
//

#include "casting.h"
#include "variables.h"
#include "myMath.h"
#include "vector.h"
#include "gpuSetting.h"

#include <cmath>
#include <OpenCL/opencl.h>
#include <time.h>
#include <stdio.h>
#include <iostream>

void bilinearTrace(float xx, float yy, unsigned int z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y){
    bool flagInt = false;
    unsigned int p;
    if ((std::ceil(xx) == std::floor(xx))){
        if ((std::ceil(yy) == std::floor(yy))) {
            flagInt = true;
        }
    }
    if ((xx < 0) || (yy < 0) || (xx >= WIDTH - 1) || ((yy >= HEIGHT - 1))){
        xx = checkRange(xx, WIDTH);
        yy = checkRange(yy, WIDTH);
        flagEnd = true;
        p = ((int)std::floor(xx) + (int)std::floor(yy) * WIDTH + z * WIDTH * HEIGHT);
        multiVec(&lColor[3 * p], &fColor[3 * p], color);
        setVec3Float(medium, &fColor[3 * p]);
//        reflectivity = fRefrecttivity[p];
        opacity = fOpacity[p];
        if (opacity == 0){
            opacity = 1;
            medium[0] = 1;
            medium[1] = 1;
            medium[2] = 1;
        }
        setVec3Float(gradient, &grad[3 * p]);
        intensity = lIntensity[p];

//        if ((x == AOIx) && (y == AOIy)){
//            std::cout << "bilinear0 " << xx << " " << yy << " " << lColor[3 * p] << lColor[3 * p+1] << lColor[3 * p+2] << std::endl;
//        }
    } else if (flagInt) {
        p = ((int)xx + (int)yy * WIDTH + z * WIDTH * HEIGHT);
        multiVec(&lColor[3 * p], &fColor[3 * p], color);
        setVec3Float(medium, &fColor[3 * p]);
//        reflectivity = fRefrectivity[p];
        opacity = fOpacity[p];
        setVec3Float(gradient, &grad[3 * p]);
        intensity = lIntensity[p];
//        if ((x == AOIx) && (y == AOIy)){
//            std::cout << "bilinear1 " << lColor[p] << std::endl;
//        }
    } else {
//        std::cout << "case 3" << z << std::endl;
        float x1[] = {std::floor(xx), std::floor(yy), (float)z};
        float x2[] = {std::floor(xx), std::floor(yy) + 1, (float)z};
        float y1[] = {std::floor(xx) + 1, std::floor(yy), (float)z};
        float y2[] = {std::floor(xx) + 1, std::floor(yy) + 1, (float)z};

        float ratiox = std::abs(xx - std::floor(xx)) / STEP;
        float ratioy = std::abs(yy - std::floor(yy)) / STEP;

//        if ((x == AOIx) && (y == AOIy)){
//            std::cout << lColor[getCoordinate(x1)] << lColor[getCoordinate(x2)] << color << std::endl;
//            std::cout << field[getCoordinate(x1)].color << field[getCoordinate(x2)].color << field[getCoordinate(y1)].color << field[getCoordinate(y2)].color << std::endl;
//            std::cout << x1[0] << " " << x1[1] << " " << y2[0] << " " << y2[1] << " "  << std::endl;
//            std::cout << fOpacity[getScalarCoo(x1)] << std::endl;
//        }
        float particle[3], vec1[3], vec2[3], vec3[3], vec4[3];

        multiFloat(&fColor[getCoordinate(x1)], (1 - ratiox), vec1);
        multiFloat(&fColor[getCoordinate(y1)], ratiox, vec2);
        multiFloat(&fColor[getCoordinate(x2)], (1 - ratiox), vec3);
        multiFloat(&fColor[getCoordinate(y2)], ratiox, vec4);
        addVec(vec1, vec2, vec1);
        addVec(vec3, vec4, vec2);
        multiFloat(vec1, 1 - ratioy, vec1);
        multiFloat(vec2, ratioy, vec2);
        addVec(vec1, vec2, particle);

        multiFloat(&lColor[getCoordinate(x1)], (1 - ratiox), vec1);
        multiFloat(&lColor[getCoordinate(y1)], ratiox, vec2);
        multiFloat(&lColor[getCoordinate(x2)], (1 - ratiox), vec3);
        multiFloat(&lColor[getCoordinate(y2)], ratiox, vec4);
        addVec(vec1, vec2, vec1);
        addVec(vec3, vec4, vec2);
        multiFloat(vec1, 1 - ratioy, vec1);
        multiFloat(vec2, ratioy, vec2);
        addVec(vec1, vec2, color);
        color[0] = 1;
        color[1] = 1;
        color[2] = 1;
        multiVec(color, particle, color);

//        if ((x == AOIx) && (y == AOIy)){
//            std::cout << "bilinear2 " << particle[0] << " " << particle[1] << " " << particle[2] << " " << ratiox << std::endl;
//            std::cout << color[0] << " " << color[1] << " " << color[2] << " " << ratiox << std::endl;
//        }

        multiFloat(&fMedium[getCoordinate(x1)], (1 - ratiox), vec1);
        multiFloat(&fMedium[getCoordinate(y1)], ratiox, vec2);
        multiFloat(&fMedium[getCoordinate(x2)], (1 - ratiox), vec3);
        multiFloat(&fMedium[getCoordinate(y2)], ratiox, vec4);
        addVec(vec1, vec2, vec1);
        addVec(vec3, vec4, vec2);
        multiFloat(vec1, 1 - ratioy, vec1);
        multiFloat(vec2, ratioy, vec2);
        addVec(vec1, vec2, medium);

//        reflectivity = (fReflectivity[getCoordinate(x1)] * (1 - ratiox) + fReflectivity[getCoordinate(y1)] * ratiox) * (1 - ratioy) + (fReflectivity[getCoordinate(x2)] * (1 - ratiox) + fReflectivity[getCoordinate(y2)] * ratiox) * ratioy;
//        opacity = (fOpacity[getScalarCoo(x1)] * (1 - ratiox) + fOpacity[getScalarCoo(y1)] * ratiox) * (1 - ratioy) + (fOpacity[getScalarCoo(x2)] * (1 - ratiox) + fOpacity[getScalarCoo(y2)] * ratiox) * ratioy;
        opacity = fOpacity[getScalarCoo(x1)];

        multiFloat(&grad[getCoordinate(x1)], (1 - ratiox), vec1);
        multiFloat(&grad[getCoordinate(y1)], ratiox, vec2);
        multiFloat(&grad[getCoordinate(x2)], (1 - ratiox), vec3);
        multiFloat(&grad[getCoordinate(y2)], ratiox, vec4);
        addVec(vec1, vec2, vec1);
        addVec(vec3, vec4, vec2);
        multiFloat(vec1, 1 - ratioy, vec1);
        multiFloat(vec2, ratioy, vec2);
        addVec(vec1, vec2, gradient);

        intensity = (lIntensity[getScalarCoo(x1)] * (1 - ratiox) + lIntensity[getScalarCoo(y1)] * ratiox) * (1 - ratioy) + (lIntensity[getScalarCoo(x2)] * (1 - ratiox) + lIntensity[getScalarCoo(y2)] * ratiox) * ratioy;
    }
}

void trace(float *angle, float x, float y, float *pixel) {
    float xx = x, yy = y;
    unsigned int z = 0;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, opacity = 0;
    while (z < DEPTH){
        float newColor[3], newMedium[3], newReflectivity, newGradient[3], newIntensity, newOpacity;
        bool flagEnd = false;

        bilinearTrace(xx, yy, z, newColor, newMedium, newReflectivity, newOpacity, newGradient, newIntensity, flagEnd, x, y);
        float vec1[3], vec2[3];
        // update color += medium * (1 - opacity) * newOpacity * newColor;
        multiFloat(medium, (1 - opacity) * newOpacity, vec1);
        multiVec(vec1, newColor, vec1);
        addVec(color, vec1, color);

//      update medium *= newMedium;
        multiVec(medium, newMedium, medium);
        opacity += (1 - opacity) * newOpacity;

//        if (z == DEPTH - 1){
//            std::cout << color << newColor << medium << newMedium << opacity << newOpacity << flagEnd << std::endl;
//        }
//        if ((x == AOIx) && (y == AOIy)){
//            std::cout << "result" << z  << " "; // << opacity << " " << newOpacity << " " << newColor[0] << newColor[1] << newColor[2] << " " << color[0] << color[1] << color[2] << std::endl;
//            std::cout << xx << " " << yy << " " << angle[0] << " " << angle[1] << " " << angle[2] << "  " << newGradient[0] << " " << newGradient[1] << " "  << newGradient[2] << " " << std::endl;
//        }

//        if ((std::abs(x - AOIx) < 3) && (std::abs(y - AOIy) < 3)){
//            color[0] = 0.4;
//            color[1] = 0.4;
//            color[2] = 1;
//        }
        if (flagEnd){
//            if ((x == AOIx) && (y == AOIy)) {
//                std::cout << "break1 " << z << " " << opacity << " " << newOpacity << std::endl;
//            }
            break;
        }
        if (std::abs(opacity - 1.0) < 0.05){
//            if ((x == AOIx) && (y == AOIy)){
//                std::cout << "break2 " << z << ", " << opacity  << ", " << newOpacity << std::endl;
//            }
            break;
        }
//      update angle +=  newGradient;
        addVec(angle, newGradient, angle);
//        angle = angle * (1 / angle.z);
        xx += angle[0] * (1 / angle[2]);
        yy += angle[1] * (1 / angle[2]);
        z++;
    }
    setVec3Float(pixel, color);
}

void biInter(float* tf, float* bf, float* tb, float* bb, float ratioy, float ratioz, float* value){
    float t[3], b[3];
    t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
    t[1] = tf[1] * (1 - ratioz) + tb[1] * ratioz;
    t[2] = tf[2] * (1 - ratioz) + tb[2] * ratioz;
    b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
    b[1] = bf[1] * (1 - ratioz) + bb[1] * ratioz;
    b[2] = bf[2] * (1 - ratioz) + bb[2] * ratioz;

    value[0] = t[0] * (1 - ratioy) + b[0] * ratioy;
    value[1] = t[1] * (1 - ratioy) + b[1] * ratioy;
    value[2] = t[2] * (1 - ratioy) + b[2] * ratioy;
}

void trilinearTrace(float xx, float yy, float z, float *color, float *medium, float &reflectivity, float &opacity, float *gradient, float &intensity, bool &flagEnd, int x, int y){
    unsigned int p;
    if ((xx < 0) || (yy < 0) | (z < 0) || (xx >= WIDTH - 1) || (yy >= HEIGHT - 1) || (z >= DEPTH - 1)) {
        xx = checkRange(xx, WIDTH);
        yy = checkRange(yy, WIDTH);
        z = checkRange(z, WIDTH);
        flagEnd = true;
        p = ((int) (xx) + (int) (yy) * WIDTH + (int) z * WIDTH * HEIGHT);
        opacity = fOpacity[p];
        medium[0] = fColor[3 * p];
        medium[1] = fColor[3 * p + 1];
        medium[2] = fColor[3 * p + 2];
        if (opacity == 0) {
            opacity = 1;
            medium[0] = 1;
            medium[1] = 1;
            medium[2] = 1;
        }
        color[0] = 1;
        color[1] = 1;
        color[2] = 1;
        color[0] = color[0] * medium[0];
        color[1] = color[1] * medium[1];
        color[2] = color[2] * medium[2];
        intensity = lIntensity[p];
    } else {
        float ltf[] = {std::floor(xx), std::floor(yy), std::floor(z)};
        float lbf[] = {std::floor(xx), std::floor(yy) + 1, std::floor(z)};
        float rtf[] = {std::floor(xx) + 1, std::floor(yy), std::floor(z)};
        float rbf[] = {std::floor(xx) + 1, std::floor(yy) + 1, std::floor(z)};
        float ltb[] = {std::floor(xx), std::floor(yy), std::floor(z) + 1};
        float lbb[] = {std::floor(xx), std::floor(yy) + 1, std::floor(z) + 1};
        float rtb[] = {std::floor(xx) + 1, std::floor(yy), std::floor(z) + 1};
        float rbb[] = {std::floor(xx) + 1, std::floor(yy) + 1, std::floor(z) + 1};

        float ratiox = std::abs(xx - std::floor(xx));
        float ratioy = std::abs(yy - std::floor(yy));
        float ratioz = std::abs(z - std::floor(z));

        float tf[3], bf[3], tb[3], bb[3], t[3], b[3];

        //        tf[0] = fColor[getCoordinate(ltf)] * (1 - ratiox) + fColor[getCoordinate(rtf)] * ratiox;
        //        tf[1] = fColor[getCoordinate(ltf) + 1] * (1 - ratiox) + fColor[getCoordinate(rtf) + 1] * ratiox;
        //        tf[2] = fColor[getCoordinate(ltf) + 2] * (1 - ratiox) + fColor[getCoordinate(rtf) + 2] * ratiox;
        //        bf[0] = fColor[getCoordinate(lbf)] * (1 - ratiox) + fColor[getCoordinate(rbf)] * ratiox;
        //        bf[1] = fColor[getCoordinate(lbf) + 1] * (1 - ratiox) + fColor[getCoordinate(rbf) + 1] * ratiox;
        //        bf[2] = fColor[getCoordinate(lbf) + 2] * (1 - ratiox) + fColor[getCoordinate(rbf) + 2] * ratiox;
        //        tb[0] = fColor[getCoordinate(ltb)] * (1 - ratiox) + fColor[getCoordinate(rtb)] * ratiox;
        //        tb[1] = fColor[getCoordinate(ltb) + 1] * (1 - ratiox) + fColor[getCoordinate(rtb) + 1] * ratiox;
        //        tb[2] = fColor[getCoordinate(ltb) + 2] * (1 - ratiox) + fColor[getCoordinate(rtb) + 2] * ratiox;
        //        bb[0] = fColor[getCoordinate(lbb)] * (1 - ratiox) + fColor[getCoordinate(rbb)] * ratiox;
        //        bb[1] = fColor[getCoordinate(lbb) + 1] * (1 - ratiox) + fColor[getCoordinate(rbb) + 1] * ratiox;
        //        bb[2] = fColor[getCoordinate(lbb) + 2] * (1 - ratiox) + fColor[getCoordinate(rbb) + 2] * ratiox;
        //        biInter(tf, bf, tb, bb, ratioy, ratioz, medium);
        medium[0] = fColor[getCoordinate(ltf)];
        medium[1] = fColor[getCoordinate(ltf) + 1];
        medium[2] = fColor[getCoordinate(ltf) + 2];

        tf[0] = lColor[getCoordinate(ltf)] * (1 - ratiox) + lColor[getCoordinate(rtf)] * ratiox;
        tf[1] = lColor[getCoordinate(ltf) + 1] * (1 - ratiox) + lColor[getCoordinate(rtf) + 1] * ratiox;
        tf[2] = lColor[getCoordinate(ltf) + 2] * (1 - ratiox) + lColor[getCoordinate(rtf) + 2] * ratiox;
        bf[0] = lColor[getCoordinate(lbf)] * (1 - ratiox) + lColor[getCoordinate(rbf)] * ratiox;
        bf[1] = lColor[getCoordinate(lbf) + 1] * (1 - ratiox) + lColor[getCoordinate(rbf) + 1] * ratiox;
        bf[2] = lColor[getCoordinate(lbf) + 2] * (1 - ratiox) + lColor[getCoordinate(rbf) + 2] * ratiox;
        tb[0] = lColor[getCoordinate(ltb)] * (1 - ratiox) + lColor[getCoordinate(rtb)] * ratiox;
        tb[1] = lColor[getCoordinate(ltb) + 1] * (1 - ratiox) + lColor[getCoordinate(rtb) + 1] * ratiox;
        tb[2] = lColor[getCoordinate(ltb) + 2] * (1 - ratiox) + lColor[getCoordinate(rtb) + 2] * ratiox;
        bb[0] = lColor[getCoordinate(lbb)] * (1 - ratiox) + lColor[getCoordinate(rbb)] * ratiox;
        bb[1] = lColor[getCoordinate(lbb) + 1] * (1 - ratiox) + lColor[getCoordinate(rbb) + 1] * ratiox;
        bb[2] = lColor[getCoordinate(lbb) + 2] * (1 - ratiox) + lColor[getCoordinate(rbb) + 2] * ratiox;
        biInter(tf, bf, tb, bb, ratioy, ratioz, color);
        color[0] = 1;
        color[1] = 1;
        color[2] = 1;
        color[0] = color[0] * medium[0];
        color[1] = color[1] * medium[1];
        color[2] = color[2] * medium[2];

        //    reflectivity = fReflectivity[getScalarCoo(ltf)];
        opacity = fOpacity[getScalarCoo(ltf)];

        tf[0] = grad[getCoordinate(ltf)] * (1 - ratiox) + grad[getCoordinate(rtf)] * ratiox;
        tf[1] = grad[getCoordinate(ltf) + 1] * (1 - ratiox) + grad[getCoordinate(rtf) + 1] * ratiox;
        tf[2] = grad[getCoordinate(ltf) + 2] * (1 - ratiox) + grad[getCoordinate(rtf) + 2] * ratiox;
        bf[0] = grad[getCoordinate(lbf)] * (1 - ratiox) + grad[getCoordinate(rbf)] * ratiox;
        bf[1] = grad[getCoordinate(lbf) + 1] * (1 - ratiox) + grad[getCoordinate(rbf) + 1] * ratiox;
        bf[2] = grad[getCoordinate(lbf) + 2] * (1 - ratiox) + grad[getCoordinate(rbf) + 2] * ratiox;
        tb[0] = grad[getCoordinate(ltb)] * (1 - ratiox) + grad[getCoordinate(rtb)] * ratiox;
        tb[1] = grad[getCoordinate(ltb) + 1] * (1 - ratiox) + grad[getCoordinate(rtb) + 1] * ratiox;
        tb[2] = grad[getCoordinate(ltb) + 2] * (1 - ratiox) + grad[getCoordinate(rtb) + 2] * ratiox;
        bb[0] = grad[getCoordinate(lbb)] * (1 - ratiox) + grad[getCoordinate(rbb)] * ratiox;
        bb[1] = grad[getCoordinate(lbb) + 1] * (1 - ratiox) + grad[getCoordinate(rbb) + 1] * ratiox;
        bb[2] = grad[getCoordinate(lbb) + 2] * (1 - ratiox) + grad[getCoordinate(rbb) + 2] * ratiox;
        biInter(tf, bf, tb, bb, ratioy, ratioz, gradient);

        tf[0] = lIntensity[getScalarCoo(ltf)] * (1 - ratiox) + lIntensity[getScalarCoo(rtf)] * ratiox;
        bf[0] = lIntensity[getScalarCoo(lbf)] * (1 - ratiox) + lIntensity[getScalarCoo(rbf)] * ratiox;
        tb[0] = lIntensity[getScalarCoo(ltb)] * (1 - ratiox) + lIntensity[getScalarCoo(rtb)] * ratiox;
        bb[0] = lIntensity[getScalarCoo(lbb)] * (1 - ratiox) + lIntensity[getScalarCoo(rbb)] * ratiox;
        t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
        b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
        intensity = t[0] * (1 - ratioy) + b[0] * ratioy;
    }
}


void trace3d(float *angle, float x, float y, float *pixel) {
    float xx = x, yy = y, z = 0;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, opacity = 0, path[3];
    while (z < DEPTH){
        float newColor[3], newMedium[3], newReflectivity, newGradient[3], newIntensity, newOpacity;
        bool flagEnd = false;

        trilinearTrace(xx, yy, z, newColor, newMedium, newReflectivity, newOpacity, newGradient, newIntensity, flagEnd, x, y);
        float vec1[3], vec2[3];
        // update color += medium * (1 - opacity) * newOpacity * newColor;
        multiFloat(medium, (1 - opacity) * newOpacity, vec1);
        multiVec(vec1, newColor, vec1);
        addVec(color, vec1, color);

//      update medium *= newMedium;
        multiVec(medium, newMedium, medium);
        opacity += (1 - opacity) * newOpacity;

//        if (z == DEPTH - 1){
//            std::cout << color << newColor << medium << newMedium << opacity << newOpacity << flagEnd << std::endl;
//        }
//        if ((x == AOIx) && (y == AOIy)){
//            std::cout << "result" << " " << xx << " " << yy << " " << z  << " " << newGradient[2] << flagEnd << std::endl;// << opacity << " " << newOpacity << " " << newColor[0] << newColor[1] << newColor[2] << " " << color[0] << color[1] << color[2] << std::endl;
//            std::cout << xx << " " << yy << " ";// << angle[0] << " " << angle[1] << " " << angle[2] << "  " << newGradient[0] << " " << newGradient[1] << " "  << newGradient[2] << " " << std::endl;
//        }
//
//        if ((std::abs(x - AOIx) < 3) && (std::abs(y - AOIy) < 3)){
//            color[0] = 0.4;
//            color[1] = 0.4;
//            color[2] = 1;
//        }
        if (flagEnd){
//            if ((x == AOIx) && (y == AOIy)) {
//                std::cout << "break1 " << z << " " << opacity << " " << newOpacity << std::endl;
//            }
            break;
        }
        if (std::abs(opacity - 1.0) < 0.05){
//            if ((x == AOIx) && (y == AOIy)){
//                std::cout << "break2 " << z << ", " << opacity  << ", " << newOpacity << std::endl;
//            }
            break;
        }
//      update angle +=  newGradient;
        multiFloat(newGradient, 100 / WIDTH, newGradient);
        addVec(angle, newGradient, angle);
        vecNormalize(angle, path);
//        angle = angle * (1 / angle.z);
        xx += path[0];
        yy += path[1];
        z += path[2];
    }
    setVec3Float(pixel, color);
}

// view point is always (0, 0, -distance)
void render(float distance, float *pixel) {
    float angle[3], dot[3];
    float xx, yy;
    for (int y = 0; y < viewH; ++y) {
        for (int x = 0; x < viewW; ++x) {
            xx = (float)x * WIDTH / viewW;
            yy = (float)y * HEIGHT / viewH;
            angle[0] = (xx - WIDTH/2) / distance;
            angle[1] = (yy - HEIGHT/2) / distance;
            angle[2] = 1;
            trace3d(angle, xx, yy, dot);
            setVec3Float(&pixel[(x + y * viewW) * 3], dot);
        }
    }
}

void gpuRender(float distance, float *pixel){
    cl_int side = WIDTH;
    cl_int dis = (int)distance;
    cl_int w = viewW;
    cl_int h = viewH;

    float* checker;
    checker = (float *)malloc(sizeof(float) * w * h);

    clock_t start,end;
    printf("start\n");
    start = clock();

    // プラットフォーム一覧を取得
    cl_platform_id platforms[PLATFORM_MAX];
    cl_uint platformCount;

    EC(clGetPlatformIDs(PLATFORM_MAX, platforms, &platformCount), "clGetPlatformIDs");
    if (platformCount == 0) {
        std::cerr << "No platform.\n";
        exit(1);
    }

    // 見つかったプラットフォームの情報を印字
    for (int i = 0; i < platformCount; i++) {
        char vendor[100] = {0};
        char version[100] = {0};
        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, nullptr), "clGetPlatformInfo");
        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, nullptr), "clGetPlatformInfo");
        std::cout << "Platform id: " << platforms[i] << ", Vendor: " << vendor << ", Version: " << version << "\n";
    }

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
    }

    // 見つかったデバイスの情報を印字
    std::cout << deviceCount << " device(s) found.\n";
    for (int i = 0; i < deviceCount; i++) {
        char name[100] = {0};
        size_t len;
        EC(clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(name), name, &len), "clGetDeviceInfo");
        std::cout << "Device id: " << i << ", Name: " << name << "\n";
    }

    // コンテキストの作成
    cl_context ctx = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    EC2("clCreateContext");

    FILE *fp;
//    char fileName[] = "./trace.cl";
    char fileName[] = "./trace3d.cl";
    fp = fopen(fileName, "r");
    if(!fp) {
        std::cout << "no file.\n";
        exit(1);
    }

    char *source_str;
    size_t source_size;
    source_str = (char*)malloc(0x10000);
    source_size = fread(source_str, 1, 0x10000, fp);
    fclose(fp);

    // プログラムのビルド
    cl_program program = NULL;
    program = clCreateProgramWithSource(ctx, 1, (const char **)&source_str, (const size_t *)&source_size, &err);
    clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    // カーネルの作成
    cl_kernel kernel = clCreateKernel(program, "trace", &err);
    EC2("clCreateKernel");

    // コマンドキューの作成
    cl_command_queue q = clCreateCommandQueue(ctx, devices[0], 0, &err);
    EC2("clCreateCommandQueue");

    cl_mem memGrad = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, grad, &err);
    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fOpacity, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memlIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, lIntensity, &err);
    cl_mem memPixel = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW*3, pixel, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW, checker, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memGrad), "clSetKernelArg0");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memfColor), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memlColor), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memfOpacity), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memlIntensity), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_mem), &memPixel), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_int), &dis), "clSetKernelAjrg6");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_int), &side), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_mem), &memChecker), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_int), &w), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_int), &h), "clSetKernelArg7");

    size_t global_item_size = w * h;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memPixel, CL_TRUE, 0, sizeof(float) * w * h * 3, pixel, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * w * h, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

    for (int position=0; position<w; ++position){
        std::cout << checker[position + h * 70/ 100 * w] << " ";
    }

    printf("\n");

    printf("\n");
    end = clock();
    printf("%lf秒かかりました\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memGrad), "clReleaseMemObject");
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfOpacity), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memlIntensity), "clReleaseMemObject");
    EC(clReleaseMemObject(memPixel), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    printf("trace end");
}