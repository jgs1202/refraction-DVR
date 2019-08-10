//
// Created by Nozomi on 2019-07-06.
//

#include "lightSetting.h"
#include "variables.h"
#include "myMath.h"
#include "gpuSetting.h"
#include "visualize.h"

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>


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

void visDirections(int photonNumber) {
    printf("visualizing directions...\n");
    std::uniform_real_distribution<double> rnd(0.0,1.0);
    std::random_device rd;
    std::mt19937 mt(rd());
    int magnify = 5, magArrow = 10;

    cv::Scalar white(255, 255, 255);
    cv::Mat pdX(cv::Size(WIDTH * 5, HEIGHT * 5), CV_8UC3, white);
    cv::Mat pdZ(cv::Size(WIDTH * 5, HEIGHT * 5), CV_8UC3, white);

    int px, py, pz, sx, sy, ex, ey;
    float dx, dy, thre = 0.3;
    for (int p=0; p<photonNumber; ++p) {
        px = (int) photonPosition[3 * p];
        py = (int) photonPosition[3 * p + 1];
        pz = (int) photonPosition[3 * p + 2];
        if ((px == plainX) && checkPosition(px, py, pz, WIDTH)) {
//            dx = photonDirection[3 * p + 2] / fRefractivity[px + py * WIDTH + pz * WIDTH * WIDTH] + photonGradDirection[3 * p + 2];
//            dy = photonDirection[3 * p + 1] / fRefractivity[px + py * WIDTH + pz * WIDTH * WIDTH] + photonGradDirection[3 * p + 1];
            dx = photonDirection[3 * p + 2];
            dy = photonDirection[3 * p + 1];
            if ((dx*dx + dy*dy > 0.1) && (rnd(mt) < thre)) {
                sx = pz * magnify, sy = py * magnify;
                ex = sx + dx * magArrow * magnify;
                ey = sy + dy * magArrow * magnify;
                cv::arrowedLine(pdX, cv::Point(sx, sy), cv::Point(ex, ey), cv::Scalar(255, 0, 0), 1, 4);
            }
        }
        if ((pz == plainZ) && checkPosition(px, py, pz, WIDTH)) {
//            dx = photonDirection[3 * p + 0] / fRefractivity[px + py * WIDTH + pz * WIDTH * WIDTH] + photonGradDirection[3 * p + 0];
//            dy = photonDirection[3 * p + 1] / fRefractivity[px + py * WIDTH + pz * WIDTH * WIDTH] + photonGradDirection[3 * p + 1];
            dx = photonDirection[3 * p + 0];
            dy = photonDirection[3 * p + 1];
            if ((dx * dx + dy * dy > 0.1) && (rnd(mt) < thre)) {
                sx = px * magnify, sy = py * magnify;
                ex = sx + dx * magArrow * magnify;
                ey = sy + dy * magArrow * magnify;
                cv::arrowedLine(pdZ, cv::Point(sx, sy), cv::Point(ex, ey), cv::Scalar(255, 0, 0), 1, 4);
            }
        }
    }
    imwrite("images/photonDirectionX.png", pdX);
    imwrite("images/photonDirectionZ.png", pdZ);
    printf("img written\n");
}

void visIntensity() {
    int magnify = 5, px, pz;
    cv::Scalar white(255, 255, 255, 255);
    cv::Mat heatmapX(cv::Size(WIDTH * magnify, HEIGHT * magnify), CV_8UC4, white);
    cv::Mat heatmapZ(cv::Size(WIDTH * magnify, HEIGHT * magnify), CV_8UC4, white);
    float max = 0;
    for (int i=0; i<WIDTH; ++i) {
        if (max < lIntensity[i]) {
            max = lIntensity[i];
        }
    }

    for (int x=0; x<WIDTH; ++x) {
        for (int y=0; y<WIDTH; ++y) {
            px = plainX + y * WIDTH + x * WIDTH * WIDTH;
            pz = x + y * WIDTH + plainZ * WIDTH * WIDTH;
            for (int s=0; s<magnify; ++s) {
                for (int t=0; t<magnify; ++t) {
                    heatmapX.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[0] = lColor[px * 3 + 2] * 255;
                    heatmapX.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[1] = lColor[px * 3 + 1] * 255;
                    heatmapX.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[2] = lColor[px * 3 + 0] * 255;
                    if (lIntensity[px] == 0.) {
                        heatmapX.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[3] = 255;
                    } else {
                        heatmapX.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[3] = lIntensity[px] / max * 255;
                    }

                    heatmapZ.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[0] = lColor[pz * 3 + 2] * 255;
                    heatmapZ.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[1] = lColor[pz * 3 + 1] * 255;
                    heatmapZ.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[2] = lColor[pz * 3 + 0] * 255;
                    if (lIntensity[pz] == 0.) {
                        heatmapZ.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[3] = 255;
                    } else {
                        heatmapZ.at<cv::Vec4b>(x * magnify + y * magnify * WIDTH * magnify + s + t * magnify * WIDTH)[3] = lIntensity[pz] / max * 255;
                    }
                }
            }
        }
    }

    imwrite("./images/light_intensity_X.png", heatmapX);
    imwrite("./images/light_intensity_Z.png", heatmapZ);
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
    std::cout << "photon mapping..." << std::endl;
    clock_t start,end;
    start = clock();
    
    cl_int side = WIDTH;
    cl_float lightIntensity = intensity / photonNumber;

    float* checker;
    checker = (float *)malloc(sizeof(float) * photonNumber);
    for (int i=0; i<photonNumber; ++i) {
        checker[i] = 0;
    }

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
    cl_mem memInitialColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*3, color, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fOpacity, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber, checker, &err);
    EC2("clCreateBuffer");

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memGrad), "clSetKernelArg0");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memDirection), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memPosition), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memIrradiance), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memlColor), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_mem), &memfColor), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_mem), &memInitialColor), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_mem), &memfOpacity), "clSeArg7tKernelAjrg6");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_int), &side), "clSetKernel");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_mem), &memChecker), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_float), &lightIntensity), "clSetKernelArg7");

    size_t global_item_size = photonNumber;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memIrradiance, CL_TRUE, 0, sizeof(float) * photonNumber, photonIrradiance, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memlColor, CL_TRUE, 0, sizeof(float) * FIELDSZ * 3, lColor, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * photonNumber, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    for (int position=0; position<WIDTH; ++position){
//        std::cout << checker[position] << " ";
//    }
//    printf("\n");

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
    std::cout << photonNumber << std::endl;

    float* photonPosition = new float[photonNumber * 3];
    float* photonDirection= new float[photonNumber * 3];

    arrangePhotonEqually(photonPosition, photonDirection, position, color, intensity, photonNumber);
    photonMapping(photonPosition, photonDirection, position, color, intensity, photonNumber);
}

void triGrad(float x, float y, float z, float *newGrad, int side) {
    int ltf[] = {(int)(x), (int)(y), (int)(z)};
    int lbf[] = {(int)(x), (int)(y) + 1, (int)(z)};
    int rtf[] = {(int)(x) + 1, (int)(y), (int)(z)};
    int rbf[] = {(int)(x) + 1, (int)(y) + 1, (int)(z)};
    int ltb[] = {(int)(x), (int)(y), (int)(z) + 1};
    int lbb[] = {(int)(x), (int)(y) + 1, (int)(z) + 1};
    int rtb[] = {(int)(x) + 1, (int)(y), (int)(z) + 1};
    int rbb[] = {(int)(x) + 1, (int)(y) + 1, (int)(z) + 1};
    float ratiox = abs(x - (float)((int)x));
    float ratioy = abs(y - (float)((int)y));
    float ratioz = abs(z - (float)((int)z));
    float tf[3], bf[3], tb[3], bb[3], t[3], b[3];
    tf[0] = grad[getCoordinateInt(ltf) * 3] * (1 - ratiox) + grad[getCoordinateInt(rtf) * 3] * ratiox;
    tf[1] = grad[getCoordinateInt(ltf) * 3 + 1] * (1 - ratiox) + grad[getCoordinateInt(rtf) * 3 + 1] * ratiox;
    tf[2] = grad[getCoordinateInt(ltf) * 3 + 2] * (1 - ratiox) + grad[getCoordinateInt(rtf) * 3 + 2] * ratiox;
    bf[0] = grad[getCoordinateInt(lbf) * 3] * (1 - ratiox) + grad[getCoordinateInt(rbf) * 3] * ratiox;
    bf[1] = grad[getCoordinateInt(lbf) * 3 + 1] * (1 - ratiox) + grad[getCoordinateInt(rbf) * 3 + 1] * ratiox;
    bf[2] = grad[getCoordinateInt(lbf) * 3 + 2] * (1 - ratiox) + grad[getCoordinateInt(rbf) * 3 + 2] * ratiox;
    tb[0] = grad[getCoordinateInt(ltb) * 3] * (1 - ratiox) + grad[getCoordinateInt(rtb) * 3] * ratiox;
    tb[1] = grad[getCoordinateInt(ltb) * 3 + 1] * (1 - ratiox) + grad[getCoordinateInt(rtb) * 3 + 1] * ratiox;
    tb[2] = grad[getCoordinateInt(ltb) * 3 + 2] * (1 - ratiox) + grad[getCoordinateInt(rtb) * 3 + 2] * ratiox;
    bb[0] = grad[getCoordinateInt(lbb) * 3] * (1 - ratiox) + grad[getCoordinateInt(rbb) * 3] * ratiox;
    bb[1] = grad[getCoordinateInt(lbb) * 3 + 1] * (1 - ratiox) + grad[getCoordinateInt(rbb) * 3 + 1] * ratiox;
    bb[2] = grad[getCoordinateInt(lbb) * 3 + 2] * (1 - ratiox) + grad[getCoordinateInt(rbb) * 3 + 2] * ratiox;
    t[0] = tf[0] * (1 - ratioz) + tb[0] * ratioz;
    t[1] = tf[1] * (1 - ratioz) + tb[1] * ratioz;
    t[2] = tf[2] * (1 - ratioz) + tb[2] * ratioz;
    b[0] = bf[0] * (1 - ratioz) + bb[0] * ratioz;
    b[1] = bf[1] * (1 - ratioz) + bb[1] * ratioz;
    b[2] = bf[2] * (1 - ratioz) + bb[2] * ratioz;
    newGrad[0] = t[0] * (1 - ratioy) + b[0] * ratioy;
    newGrad[1] = t[1] * (1 - ratioy) + b[1] * ratioy;
    newGrad[2] = t[2] * (1 - ratioy) + b[2] * ratioy;
}

void gpuParticipate(int photonNumber, int time){
    std::cout << "participating..." << std::endl;
    clock_t start,end;
    start = clock();

    float* checker;
    checker = (float *)malloc(sizeof(float) * photonNumber);

    memset(lColor, 0., sizeof(float) * FIELDSZ * 3);

    // プラットフォーム一覧を取得
    cl_platform_id platforms[PLATFORM_MAX];
    cl_uint platformCount;

    EC(clGetPlatformIDs(PLATFORM_MAX, platforms, &platformCount), "clGetPlatformIDs");

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");

    // コンテキストの作成
    cl_context ctx = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    EC2("clCreateContext");

    FILE *fp;
    char fileName[] = "./photonParticipating.cl";
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
    cl_mem memGradDirection = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber*3, photonGradDirection, &err);
    cl_mem memPosition = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber*3, photonPosition, &err);
    cl_mem mempColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber*3, photonColor, &err);
    cl_mem memlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, lColor, &err);
    cl_mem memfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ*3, fColor, &err);
    cl_mem memfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fOpacity, &err);
    cl_mem memfRefractivity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*FIELDSZ, fRefractivity, &err);
    cl_mem memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*photonNumber, checker, &err);
    EC2("clCreateBuffer");

    cl_int side = WIDTH;

    EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &memGrad), "clSetKernelArg0");
    EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &memDirection), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &memGradDirection), "clSetKernelArg1");
    EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &memPosition), "clSetKernelArg2");
    EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &mempColor), "clSetKernelArg3");
    EC(clSetKernelArg(kernel, 5, sizeof(cl_mem), &memlColor), "clSetKernelArg4");
    EC(clSetKernelArg(kernel, 6, sizeof(cl_mem), &memfColor), "clSetKernelArg5");
    EC(clSetKernelArg(kernel, 7, sizeof(cl_mem), &memfOpacity), "clSetKernelArg6");
    EC(clSetKernelArg(kernel, 8, sizeof(cl_mem), &memfRefractivity), "clSetKernelArg7");
    EC(clSetKernelArg(kernel, 9, sizeof(cl_int), &side), "clSetKernelArg8");
    EC(clSetKernelArg(kernel, 10, sizeof(cl_mem), &memChecker), "clSetKernelArg9");

    size_t global_item_size = photonNumber;
    EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

    // 結果を読み込み
    EC(clEnqueueReadBuffer(q, memlColor, CL_TRUE, 0, sizeof(float) * FIELDSZ * 3, lColor, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memGradDirection, CL_TRUE, 0, sizeof(float) * photonNumber * 3, photonGradDirection, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memDirection, CL_TRUE, 0, sizeof(float) * photonNumber * 3, photonDirection, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memPosition, CL_TRUE, 0, sizeof(float) * photonNumber * 3, photonPosition, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, mempColor, CL_TRUE, 0, sizeof(float) * photonNumber * 3, photonColor, 0, nullptr, nullptr), "clEnqueueReadBuffer");
    EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * photonNumber, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//    int iniP = photonNumber / 2;
//    for (int position=iniP; position < iniP+WIDTH; ++position){
//        std::cout << checker[position] << " ";
//    }
//    printf("\n");

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(memGrad), "clReleaseMemObject");
    EC(clReleaseMemObject(memDirection), "clReleaseMemObject");
    EC(clReleaseMemObject(memPosition), "clReleaseMemObject");
    EC(clReleaseMemObject(mempColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memfRefractivity), "clReleaseMemObject");
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

void photonParticipate(int photonNumber, int time) {
    memset(lColor, 0., sizeof(float) * FIELDSZ * 3);
    memset(lIntensity, 0., sizeof(float) * FIELDSZ);
    int calcStep = 1, p;
    bool flagEnd = true;
    for (int photon = 0; photon < photonNumber; ++photon) {
        if (time >= photonStartTime[photon]) {
            float x, y, z, dx, dy, dz, newGrad[3];
            newGrad[0] = 0;
            newGrad[1] = 0;
            newGrad[2] = 0;
            x = photonPosition[3 * photon + 0];
            y = photonPosition[3 * photon + 1];
            z = photonPosition[3 * photon + 2];
            dx = photonDirection[3 * photon + 0];
            dy = photonDirection[3 * photon + 1];
            dz = photonDirection[3 * photon + 2];

            p = (int) x + (int) y * WIDTH + (int) z * WIDTH * WIDTH;
            if (checkPosition(x, y, z, WIDTH)) {
                photonIrradiance[p] = 1;
                triGrad(x, y, z, newGrad, WIDTH);
                dx += newGrad[0] * 100 / (float) WIDTH / (float) calcStep;
                dy += newGrad[1] * 100 / (float) WIDTH / (float) calcStep;
                dz += newGrad[2] * 100 / (float) WIDTH / (float) calcStep;
                photonDirection[3 * photon + 0] = dx;
                photonDirection[3 * photon + 1] = dy;
                photonDirection[3 * photon + 2] = dz;
                if (lIntensity[p] == 0) {
                    lColor[p * 3 + 0] = photonColor[3 * photon + 0];
                    lColor[p * 3 + 1] = photonColor[3 * photon + 1];
                    lColor[p * 3 + 2] = photonColor[3 * photon + 2];
                    lIntensity[p] += 1;
                } else {
                    lColor[p * 3 + 0] = (photonColor[3 * photon + 0] + lColor[p * 3 + 0] * lIntensity[p]) / (lIntensity[p] + 1);
                    lColor[p * 3 + 1] = (photonColor[3 * photon + 1] + lColor[p * 3 + 1] * lIntensity[p]) / (lIntensity[p] + 1);
                    lColor[p * 3 + 2] = (photonColor[3 * photon + 2] + lColor[p * 3 + 2] * lIntensity[p]) / (lIntensity[p] + 1);
                    lIntensity[p] += 1;
                }
                photonColor[3 * photon + 0] *= (fMedium[3 * p + 0] * (1 - fOpacity[p]));
                photonColor[3 * photon + 1] *= (fMedium[3 * p + 1] * (1 - fOpacity[p]));
                photonColor[3 * photon + 2] *= (fMedium[3 * p + 2] * (1 - fOpacity[p]));
                flagEnd = false;
            }

            //update position
            float square = dx * dx + dy * dy + dz * dz;
            x += dx / sqrt(square) / (float) calcStep;
            y += dy / sqrt(square) / (float) calcStep;
            z += dz / sqrt(square) / (float) calcStep;
            photonPosition[3 * photon + 0] = x;
            photonPosition[3 * photon + 1] = y;
            photonPosition[3 * photon + 2] = z;
        }
    }
}

void setLight1(void){
    float lightPosition[3], lightColor[3], intensity, iniDirection[3];

    int margin, photonNumber;
    margin = 10;
    photonNumber = WIDTH * WIDTH * margin;
    photonEnd += photonNumber;
    if (photonNumber > maxPhotons) {
        std::cout << "Error: too many photons\n";
        exit(1);
    }
    memset(photonGradDirection, 0., sizeof(float) * photonNumber * 3);
    lightColor[0] = 1.;
    lightColor[1] = 1.;
    lightColor[2] = 1.;
    iniDirection[0] = 1.;
    iniDirection[1] = 0.;
    iniDirection[2] = 0.;
    int photonIndex = 0;
    for (int y=0; y<WIDTH; ++y) {
        for (int z=0; z<WIDTH; ++z) {
            for (int i=0; i<margin; ++i) {
                photonPosition[photonIndex * 3 + 0] = -i;
                photonPosition[photonIndex * 3 + 1] = y;
                photonPosition[photonIndex * 3 + 2] = z;
                photonDirection[photonIndex * 3 + 0] = iniDirection[0];
                photonDirection[photonIndex * 3 + 1] = iniDirection[1];
                photonDirection[photonIndex * 3 + 2] = iniDirection[2];
                photonColor[photonIndex * 3 + 0] = lightColor[0];
                photonColor[photonIndex * 3 + 1] = lightColor[1];
                photonColor[photonIndex * 3 + 2] = lightColor[2];
                photonStartTime[photonIndex] = 0;
                photonIndex += 1;
            }
        }
    }
}

void setLight2(void){
    float lightPosition[3], lightColor[3], intensity, iniDirection[3];

    int margin, photonNumber, duration;
    margin = 10;
    duration = 10;
    photonNumber = WIDTH * WIDTH * margin * duration;
    photonEnd += photonNumber;

    if (photonNumber > maxPhotons) {
        std::cout << "Error: too many photons\n";
        exit(1);
    }
    memset(photonGradDirection, 0., sizeof(float) * photonNumber * 3);
    lightColor[0] = 1.;
    lightColor[1] = 1.;
    lightColor[2] = 1.;

    float iniPosition[3] = {(float)WIDTH / 8, (float)WIDTH / 2, (float)WIDTH / 2};
    float theta, phi, hNum;
    phi = 0;
    for(int t=0; t<duration; ++t) {
        for (int photon = 0; photon < photonNumber / duration; ++photon) {
            photonPosition[(photonNumber * t / duration) * 3 + photon * 3] = iniPosition[0];
            photonPosition[(photonNumber * t / duration) * 3 + photon * 3 + 1] = iniPosition[1];
            photonPosition[(photonNumber * t / duration) * 3 + photon * 3 + 2] = iniPosition[2];

            hNum = 2. * (photon) / (photonNumber / duration - 1.) - 1.;
            theta = acos(hNum);
            if (photon != 0) {
                phi = phi + 3.6 / sqrt(photonNumber / duration) / sqrt(1 - hNum * hNum);
            }
            //        std::cout << photon << ": " << hNum << " " << theta << " " << phi << std::endl;
            photonDirection[(photonNumber * t / duration) * 3 + 3 * photon + 0] = sin(theta) * cos(phi);
            photonDirection[(photonNumber * t / duration) * 3 + 3 * photon + 1] = sin(theta) * sin(phi);
            photonDirection[(photonNumber * t / duration) * 3 + 3 * photon + 2] = cos(theta);
            photonColor[(photonNumber * t / duration) * 3 + 3 * photon + 0] = lightColor[0];
            photonColor[(photonNumber * t / duration) * 3 + 3 * photon + 1] = lightColor[1];
            photonColor[(photonNumber * t / duration) * 3 + 3 * photon + 2] = lightColor[2];

            photonStartTime[photonNumber * t / duration + 3 * photon] = t;
        }
    }
}