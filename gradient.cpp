//
// Created by Nozomi on 2019-05-11.
//

#include "gradient.h"
#include "variables.h"
#include "myMath.h"
#include "gpuSetting.h"
#include <OpenCL/opencl.h>
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
        a[1] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[4], x2 = around[6];
    if ((x1 != 0) && (x2 != 0)){
        a[2] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[5], x2 = around[7];
    if ((x1 != 0) && (x2 != 0)){
        a[3] = slope2(x1, x2);
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
        a[1] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[2], x2 = around[6];
    if ((x1 != 0) && (x2 != 0)){
        a[2] = slope2(x1, x2);
        count += 1;
    }
    x1 = around[3], x2 = around[7];
    if ((x1 != 0) && (x2 != 0)){
        a[3] = slope2(x1, x2);
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

void gpuSmoothing(void){
    cl_int side = WIDTH;

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
    char fileName[] = "./smoothing.cl";
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
    cl_kernel kernel = clCreateKernel(program, "smooth", &err);
    EC2("clCreateKernel");

    // コマンドキューの作成
    cl_command_queue q = clCreateCommandQueue(ctx, devices[0], 0, &err);
    EC2("clCreateCommandQueue");

    cl_mem memGrad = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, grad, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memGrad), "clSetKernelArg0");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_int), &side), "clSetKernelArg7");

    size_t global_item_size = FIELDSZ;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memGrad, CL_TRUE, 0, sizeof(float) * FIELDSZ * 3, grad, 0, nullptr, nullptr), "clEnqueueReadBuffer");

    end = clock();
//    printf("%lf秒かかりました\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memGrad), "clReleaseMemObject");
    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
//    printf("smoothing end");
}