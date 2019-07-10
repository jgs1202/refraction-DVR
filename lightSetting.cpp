//
// Created by Nozomi on 2019-07-06.
//

#include "lightSetting.h"
#include "variables.h"
#include "myMath.h"
#include "gpuSetting.h"

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>


void visPhotons(float* photonDirection, int photonNumber){
    int x, y, side;
    side = 1000;
    cv::Mat photonImage(cv::Size(side, side), CV_8U);
    for (int p=0; p<photonNumber; ++p) {
        if (photonDirection[3 * p + 2] < 0) {
            x = roundInt((photonDirection[3 * p] + 1) * side / 2);
            y = roundInt((photonDirection[3 * p + 1] + 1) * side / 2);
            photonImage.at<unsigned char>(y, x) = 255;
        }
    }
    imwrite("photon_direction.png", photonImage);
}

void arrangePhotonEqually(float* photonPosition, float* photonDirection, float* position, float* color, int intensity, int photonNumber){
    float theta, phi, hNum;
    phi = 0;
    for (int photon=0; photon<photonNumber; ++photon) {
        photonPosition[photon * 3] = position[0];
        photonPosition[photon * 3 + 1] = position[1];
        photonPosition[photon * 3 + 2] = position[2];

        hNum = 2. * (photon) / (photonNumber - 1.) - 1.;
        theta = acos(hNum);
        if (photon != 0) {
            phi = phi + 3.6 / sqrt(photonNumber) / sqrt(1 - hNum * hNum);
        }
//        std::cout << photon << ": " << hNum << " " << theta << " " << phi << std::endl;
        photonDirection[3 * photon + 0] = sin(theta) * cos(phi);
        photonDirection[3 * photon + 1] = sin(theta) * sin(phi);
        photonDirection[3 * photon + 2] = cos(theta);
    }
    visPhotons(photonDirection, photonNumber);
}

void photonMapping(float* photonPosition, float* photonDirection, float* position, float* color, float intensity, int photonNumber){
    std::cout << "rendering..." << std::endl;
    cl_int side = WIDTH;

    float* checker;
    checker = (float *)malloc(sizeof(float) * photonNumber);
    for (int i=0; i<photonNumber; ++i) {
        checker[i] = 0;
    }

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
    char fileName[] = "./photonMapping.cl";
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
    cl_mem memDirection = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber*3, photonDirection, &err);
    cl_mem memPosition = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber*3, photonPosition, &err);
    cl_mem memIrradiance = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, photonIrradiance, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fOpacity, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber, checker, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memGrad), "clSetKernelArg0");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memDirection), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memPosition), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memIrradiance), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memlColor), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_mem), &memfColor), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_mem), &memfOpacity), "clSetKernelAjrg6");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_int), &side), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_mem), &memChecker), "clSetKernelArg7");

    size_t global_item_size = photonNumber;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memIrradiance, CL_TRUE, 0, sizeof(float) * photonNumber, photonIrradiance, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memlColor, CL_TRUE, 0, sizeof(float) * FIELDSZ * 3, photonColor, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * photonNumber, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

    for (int position=0; position<WIDTH; ++position){
        std::cout << checker[position] << " ";
    }
    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memGrad), "clReleaseMemObject");
    EC(clReleaseMemObject(memDirection), "clReleaseMemObject");
    EC(clReleaseMemObject(memPosition), "clReleaseMemObject");
    EC(clReleaseMemObject(memIrradiance), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
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

void insideLight(float *position, float *color, float intensity) {
    int margin, photonNumber;
    margin = 1;
    photonNumber = WIDTH * WIDTH * 6 * margin;

    float* photonPosition = new float[photonNumber * 3];
    float* photonDirection= new float[photonNumber * 3];

    arrangePhotonEqually(photonPosition, photonDirection, position, color, intensity, photonNumber);
    photonMapping(photonPosition, photonDirection, position, color, intensity, photonNumber);
}