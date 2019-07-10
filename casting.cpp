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
#include <opencv2/opencv.hpp>

void visualize(void) {
    //  make image of the object field
    std::vector<float> gradPlainY(PLAINSZ);
    std::vector<float> gradPlainX(PLAINSZ);
    for (int z=0; z<DEPTH; ++z) {
        for (int x = 0; x < WIDTH; ++x) {
            gradPlainY[x + z * WIDTH] = grad[(x + plainY * WIDTH + z * WIDTH * HEIGHT) * 3];
            gradPlainX[x + z * WIDTH] = grad[(plainX + x * WIDTH + z * WIDTH * HEIGHT) * 3 + 1];
        }
    }
    float maxY = *std::max_element(gradPlainY.begin(), gradPlainY.end()), valueY, normalY;
    float maxX = *std::max_element(gradPlainX.begin(), gradPlainX.end()), valueX, normalX;
    cv::Mat gradAndPathsY(cv::Size(WIDTH * 5, HEIGHT * 5), CV_8UC3);
    cv::Mat gradAndPathsX(cv::Size(WIDTH * 5, HEIGHT * 5), CV_8UC3);
    cv::Mat refracIndexY(cv::Size(WIDTH, HEIGHT), CV_8UC1);
    cv::Mat refracIndexX(cv::Size(WIDTH, HEIGHT), CV_8UC1);
    for (int z=0; z<DEPTH; ++z){
        for (int x=0; x<WIDTH; ++x){
            for (int i=0; i<5; ++i){
                for (int j=0; j<5; ++j) {
                    valueY = grad[(x + plainY * WIDTH + z * WIDTH * HEIGHT) * 3];
                    normalY = valueY / maxY;
                    gradAndPathsY.at<cv::Vec3b>((x + z * 5 * WIDTH) * 5 + i + j * 5 * WIDTH)[0] = (valueY > 0)?normalY * 255 : 0;
                    gradAndPathsY.at<cv::Vec3b>((x + z * 5 * WIDTH) * 5 + i + j * 5 * WIDTH)[1] = 100 * gaussFunc((std::abs(normalY) * 255));
                    gradAndPathsY.at<cv::Vec3b>((x + z * 5 * WIDTH) * 5 + i + j * 5 * WIDTH)[2] = (valueY > 0)? 0 :  - 1 * normalY * 255;

                    valueX = grad[(plainX + x * WIDTH + z * WIDTH * HEIGHT) * 3 + 1];
                    normalX = valueX / maxX;
                    gradAndPathsX.at<cv::Vec3b>((z + x * 5 * WIDTH) * 5 + j + i * 5 * WIDTH)[0] = (valueX > 0)?normalX * 255 : 0;
                    gradAndPathsX.at<cv::Vec3b>((z + x * 5 * WIDTH) * 5 + j + i * 5 * WIDTH)[1] = 100 * gaussFunc((std::abs(normalX) * 255));
                    gradAndPathsX.at<cv::Vec3b>((z + x * 5 * WIDTH) * 5 + j + i * 5 * WIDTH)[2] = (valueX > 0)? 0 :  - 1 * normalX * 255;
                }
            }
        }
    }
    int x, z;
    for (int i=0; i<castIndexY; ++i){
        x = (int)roundInt(castPositionY[i * 2] * 5);
        z = (int)roundInt(castPositionY[i * 2 + 1] * 5);
        gradAndPathsY.at<cv::Vec3b>(x + z * 5 * WIDTH)[0] = 255;
        gradAndPathsY.at<cv::Vec3b>(x + z * 5 * WIDTH)[1] = 255;
        gradAndPathsY.at<cv::Vec3b>(x + z * 5 * WIDTH)[2] = 255;

        x = (int)roundInt(castPositionX[i * 2] * 5);
        z = (int)roundInt(castPositionX[i * 2 + 1] * 5);
        gradAndPathsX.at<cv::Vec3b>(z + x * 5 * WIDTH)[0] = 255;
        gradAndPathsX.at<cv::Vec3b>(z + x * 5 * WIDTH)[1] = 255;
        gradAndPathsX.at<cv::Vec3b>(z + x * 5 * WIDTH)[2] = 255;
    }
    float maxRefraction = 1.8;
    for (int z=0; z<DEPTH; ++z){
        for (int x=0; x<WIDTH; ++x){
            valueY = fRefractivity[(x + plainY * WIDTH + z * WIDTH * HEIGHT)];
            normalY = valueY / maxRefraction;
            refracIndexY.at<unsigned char>(x + z * WIDTH) = exponential(normalY) / exponential(1) * 255;

            valueX = fRefractivity[(plainX + x * WIDTH + z * WIDTH * HEIGHT)];
            normalX = valueX / maxRefraction;
            refracIndexX.at<unsigned char>(z + x * WIDTH) = exponential(normalX) / exponential(1) * 255;
        }
    }
    imwrite("gradY.png", gradAndPathsY);
    imwrite("gradX.png", gradAndPathsX);
    imwrite("refractive_indexY.png", refracIndexY);
    imwrite("refractive_indexX.png", refracIndexX);
    printf("img written\n");
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

//        tf[0] = lColor[getCoordinate(ltf)] * (1 - ratiox) + lColor[getCoordinate(rtf)] * ratiox;
//        tf[1] = lColor[getCoordinate(ltf) + 1] * (1 - ratiox) + lColor[getCoordinate(rtf) + 1] * ratiox;
//        tf[2] = lColor[getCoordinate(ltf) + 2] * (1 - ratiox) + lColor[getCoordinate(rtf) + 2] * ratiox;
//        bf[0] = lColor[getCoordinate(lbf)] * (1 - ratiox) + lColor[getCoordinate(rbf)] * ratiox;
//        bf[1] = lColor[getCoordinate(lbf) + 1] * (1 - ratiox) + lColor[getCoordinate(rbf) + 1] * ratiox;
//        bf[2] = lColor[getCoordinate(lbf) + 2] * (1 - ratiox) + lColor[getCoordinate(rbf) + 2] * ratiox;
//        tb[0] = lColor[getCoordinate(ltb)] * (1 - ratiox) + lColor[getCoordinate(rtb)] * ratiox;
//        tb[1] = lColor[getCoordinate(ltb) + 1] * (1 - ratiox) + lColor[getCoordinate(rtb) + 1] * ratiox;
//        tb[2] = lColor[getCoordinate(ltb) + 2] * (1 - ratiox) + lColor[getCoordinate(rtb) + 2] * ratiox;
//        bb[0] = lColor[getCoordinate(lbb)] * (1 - ratiox) + lColor[getCoordinate(rbb)] * ratiox;
//        bb[1] = lColor[getCoordinate(lbb) + 1] * (1 - ratiox) + lColor[getCoordinate(rbb) + 1] * ratiox;
//        bb[2] = lColor[getCoordinate(lbb) + 2] * (1 - ratiox) + lColor[getCoordinate(rbb) + 2] * ratiox;
//        biInter(tf, bf, tb, bb, ratioy, ratioz, color);
        color[0] = lColor[getCoordinate(ltf)];
        color[1] = lColor[getCoordinate(ltf) + 1];
        color[2] = lColor[getCoordinate(ltf) + 2];
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
    float calcStep = 1;
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

        int AOIx = 70, AOIy = 80;
        if ((x == AOIx) && (y == AOIy)){
            std::cout << "result" << " " << xx << " " << yy << " " << z  << " " << newColor[1] << " " << newOpacity << " " << flagEnd << std::endl;// << opacity << " " << newOpacity << " " << newColor[0] << newColor[1] << newColor[2] << " " << color[0] << color[1] << color[2] << std::endl;
            std::cout << (int)(newColor[0] * 100) << " " << (int)(newColor[1] * 100) << " " << (int)(newColor[2] * 100) << " " << (int)(color[0] * 100) << " " << (int)(color[1] * 100) << " " << (int)(color[2] * 100) << std::endl;
            //            std::cout << xx << " " << yy << " ";// << angle[0] << " " << angle[1] << " " << angle[2] << "  " << newGradient[0] << " " << newGradient[1] << " "  << newGradient[2] << " " << std::endl;
        }

//        if ((std::abs(x - AOIx) < 3) && (std::abs(y - AOIy) < 3)){
//            color[0] = 0.4;
//            color[1] = 0.4;
//            color[2] = 1;
//        }
        if (flagEnd){
            if ((x == AOIx) && (y == AOIy)) {
                std::cout << "break1 " << z << " " << opacity << " " << newOpacity << std::endl;
            }
            break;
        }
        if (std::abs(opacity - 1.0) < 0.001){
            if ((x == AOIx) && (y == AOIy)){
                std::cout << "break2 " << z << ", " << opacity  << ", " << newOpacity << std::endl;
            }
            break;
        }
//      update angle +=  newGradient;
        multiFloat(newGradient, 100 / (float)WIDTH, newGradient);
        multiFloat(newGradient, 1 / calcStep, newGradient);
        addVec(angle, newGradient, angle);
        vecNormalize(angle, path);
        if ((x == AOIx) && (y == AOIy)){
//            std::cout << path[0] << newGradient[0] / (float)WIDTH << std::endl;
        }

        if (y == plainY) {
            castPositionY[castIndexY * 2] = xx;
            castPositionY[castIndexY * 2 + 1] = z;
            castIndexY += 1;
        }

        if (x == plainX) {
            castPositionX[castIndexX * 2] = yy;
            castPositionX[castIndexX * 2 + 1] = z;
            castIndexX += 1;
        }

//        angle = angle * (1 / angle.z);
        xx += path[0] / calcStep;
        yy += path[1] / calcStep;
        z += path[2] / calcStep;
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
    visualize();
}

void gpuRender(float distance, float *pixel){
    std::cout << "rendering..." << std::endl;
    cl_int side = WIDTH;
    cl_int dis = (int)distance;
    cl_int w = viewW;
    cl_int h = viewH;

    float* checker;
    checker = (float *)malloc(sizeof(float) * w * h);

    clock_t start,end;
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
//    for (int i = 0; i < platformCount; i++) {
//        char vendor[100] = {0};
//        char version[100] = {0};
//        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, nullptr), "clGetPlatformInfo");
//        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, nullptr), "clGetPlatformInfo");
//        std::cout << "Platform id: " << platforms[i] << ", Vendor: " << vendor << ", Version: " << version << "\n";
//    }

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
    }

    // 見つかったデバイスの情報を印字
//    std::cout << deviceCount << " device(s) found.\n";
//    for (int i = 0; i < deviceCount; i++) {
//        char name[100] = {0};
//        size_t len;
//        EC(clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(name), name, &len), "clGetDeviceInfo");
//        std::cout << "Device id: " << i << ", Name: " << name << "\n";
//    }

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
        std::cout << checker[position + (int)(h * 70/ 100 * w)] << " ";
    }
    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

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
    free(checker);
}