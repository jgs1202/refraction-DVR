//
// Created by Nozomi on 2019-08-11.
//

#include "render.h"
#include "variables.h"
#include "myMath.h"
#include "gpuSetting.h"
#include "vector.h"

#include <math.h>
#include <iostream>
#include <cmath>

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
    if ((xx < 0) || (yy < 0) | (z < 0) || (xx > WIDTH - 1) || (yy > HEIGHT - 1) || (z > DEPTH - 1)) {
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
        color[0] = lColor[3 * p] * medium[0];
        color[1] = lColor[3 * p + 1] * medium[1];
        color[2] = lColor[3 * p + 2] * medium[2];
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

        medium[0] = fColor[getCoordinate(ltf)];
        medium[1] = fColor[getCoordinate(ltf) + 1];
        medium[2] = fColor[getCoordinate(ltf) + 2];

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

void trace3d(float *angle, float x, float y, float z, float *pixel) {
    float xx = x, yy = y;
    float color[] = {0., 0., 0.}, medium[] = {1.0, 1.0, 1.0}, opacity = 0, path[3];
    float calcStep = 1;
    while (!(xx < 0) || !(yy < 0) || !(z < 0)|| !(xx > WIDTH - 1) || !(yy > WIDTH - 1) || !(z > WIDTH - 1)){
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

        int AOIx = 30, AOIy = 60;

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

        xx += path[0] / calcStep;
        yy += path[1] / calcStep;
        z += path[2] / calcStep;
    }
    setVec3Float(pixel, color);
}


void gpuRender(float distance, float *pixel, float* rayPositions, float* rayAngles){
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

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
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
    cl_mem memRayP = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW * 3, rayPositions, &err);
    cl_mem memRayD = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW * 3, rayAngles, &err);
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
    EC(clSetKernelArg(kernel, 11, sizeof(cl_mem), &memRayP), "clSetKernelArg11");
    EC(clSetKernelArg(kernel, 12, sizeof(cl_mem), &memRayD), "clSetKernelArg12");

    size_t global_item_size = w * h;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memPixel, CL_TRUE, 0, sizeof(float) * w * h * 3, pixel, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * w * h, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    for (int position=0; position<w; ++position){
//        std::cout << checker[position + (int)(h * 70/ 100 * w)] << " ";
//    }
//    printf("\n");

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
    EC(clReleaseMemObject(memRayP), "clReleaseMemObject");
    EC(clReleaseMemObject(memRayD), "clReleaseMemObject");
    EC(clReleaseMemObject(memChecker), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    free(checker);
}


void renderlColor(float distance, float *pixel){
    std::cout << "rendering..." << std::endl;
    cl_int side = WIDTH;
    cl_int dis = (int)distance;
    cl_int w = viewW;
    cl_int h = viewH;
    cl_float sx = shiftX;
    cl_float sy = shiftY;

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

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
    }

    // コンテキストの作成
    cl_context ctx = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    EC2("clCreateContext");

    FILE *fp;
    char fileName[] = "./tracelColor.cl";
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

    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fOpacity, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memlIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, lIntensity, &err);
    cl_mem memPixel = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW*3, pixel, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW, checker, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memfColor), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memlColor), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memfOpacity), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memlIntensity), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memPixel), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_int), &dis), "clSetKernelAjrg6");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_int), &side), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_mem), &memChecker), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_int), &w), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_int), &h), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_float), &sx), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 11, sizeof(cl_float), &sy), "clSetKernelArg7");

    size_t global_item_size = w * h;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memPixel, CL_TRUE, 0, sizeof(float) * w * h * 3, pixel, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * w * h, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    for (int position=0; position<w; ++position){r
//        std::cout << lColor[position + (int)(h * 50/ 100 * w)] << " ";
//    }
//    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfOpacity), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memlIntensity), "clReleaseMemObject");
    EC(clReleaseMemObject(memPixel), "clReleaseMemObject");
    EC(clReleaseMemObject(memChecker), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    free(checker);
}

void gpuRenderlColor(float distance, float *pixel, float* rayPositions, float* rayAngles){
    std::cout << "rendering... " << std::endl;
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

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
        exit(1);
    }

    // コンテキストの作成
    cl_context ctx = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    EC2("clCreateContext");

    FILE *fp;
//    char fileName[] = "./trace.cl";
    char fileName[] = "./tracelColorRotate.cl";
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

    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fOpacity, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memlIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, lIntensity, &err);
    cl_mem memPixel = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW*3, pixel, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW, checker, &err);
    cl_mem memRayP = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW * 3, rayPositions, &err);
    cl_mem memRayD = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*viewH * viewW * 3, rayAngles, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memfColor), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memlColor), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memfOpacity), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memlIntensity), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memPixel), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_int), &dis), "clSetKernelAjrg6");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_int), &side), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_mem), &memChecker), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_int), &w), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_int), &h), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_mem), &memRayP), "clSetKernelArg11");
    EC(clSetKernelArg(kernel, 11, sizeof(cl_mem), &memRayD), "clSetKernelArg12");

    size_t global_item_size = w * h;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memPixel, CL_TRUE, 0, sizeof(float) * w * h * 3, pixel, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * w * h, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    for (int position=0; position<w; ++position){
//        std::cout << checker[position + (int)(h * 70/ 100 * w)] << " ";
//    }
//    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfOpacity), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memlIntensity), "clReleaseMemObject");
    EC(clReleaseMemObject(memPixel), "clReleaseMemObject");
    EC(clReleaseMemObject(memChecker), "clReleaseMemObject");
    EC(clReleaseMemObject(memRayP), "clReleaseMemObject");
    EC(clReleaseMemObject(memRayD), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    free(checker);
}