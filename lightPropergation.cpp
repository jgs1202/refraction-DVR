//
// Created by Nozomi on 2019-05-11.
//

#include "lightPropergation.h"
#include "variables.h"
#include "myMath.h"
#include "gpuSetting.h"
#include "vector.h"

#include <OpenCL/opencl.h>
#include <time.h>
#include <stdio.h>
#include <iostream>


int momGauss(int x, int y, int dx, int dy, int side){
    float gauss[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    if (checkRangeInt(x - 1, WIDTH) && checkRangeInt(x + 1, WIDTH) && checkRangeInt(y - 1, WIDTH) && checkRangeInt(y + 1, WIDTH)){
        return 16;
    } else if ((checkRangeInt(x - 1, WIDTH) && checkRangeInt(x + 1, WIDTH)) || (checkRangeInt(y - 1, WIDTH) && checkRangeInt(y + 1, WIDTH))){
        return 12;
    } else {
        return 9;
    }
}

int momMean(int x, int y, int dx, int dy, int side){
    float gauss[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    if (checkRangeInt(x - 1, WIDTH) && checkRangeInt(x + 1, WIDTH) && checkRangeInt(y - 1, WIDTH) && checkRangeInt(y + 1, WIDTH)){
        return 9;
    } else if ((checkRangeInt(x - 1, WIDTH) && checkRangeInt(x + 1, WIDTH)) || (checkRangeInt(y - 1, WIDTH) && checkRangeInt(y + 1, WIDTH))){
        return 6;
    } else {
        return 4;
    }
}

void lightSmoothing(float *plainData){
    float gauss[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    float* newLight = new float[PLAINSZ * 3];
    for (int y=0; y<HEIGHT; ++y){
        for (int x=0; x<WIDTH; ++x){
            for (int direction=0; direction<3; ++direction){
                float sum[3] = {0, 0, 0};
                for (int dx=0; dx<3; ++dx){
                    for (int dy=0; dy<3; ++dy) {
                        if (checkRangeInt(x + dx - 1, WIDTH) && checkRangeInt(y + dy - 1, HEIGHT)){
//                            sum[direction] += plainData[(x + dx - 1 + (y + dy - 1) * WIDTH) * 3 + direction] * gauss[dx + dy * 3] / mom(x, y, dx, dy, WIDTH);
                            sum[direction] += plainData[(x + dx - 1 + (y + dy - 1) * WIDTH) * 3 + direction] / momMean(x, y, dx, dy, WIDTH);
                        }
                    }
                }
                newLight[(x + y * WIDTH) * 3 + direction] = sum[direction];
            }
        }
    }
    for (int p=0; p<PLAINSZ*3; ++p){
        plainData[p] = newLight[p];
    }

    delete[] newLight;
}

void gpuCalcLightSource (float *direction, float *color) {
    std::cout << "propergating light..." << std::endl;
    float* pGrad;
    float* pfColor;
    float* pfMedium;
    float* pfOpacity;
    float* plColor;
    float* plDirection;
    float* plIntensity;
    float* nlColor;
    float* nlDirection;
    float* nlIntensity;
    float* checker;

    pGrad = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    pfColor = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    pfMedium = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    pfOpacity = (float *)malloc(sizeof(float) * PLAINSZ);
    plColor = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    plDirection = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    plIntensity = (float *)malloc(sizeof(float) * PLAINSZ);
    nlColor = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    nlDirection = (float *)malloc(sizeof(float) * PLAINSZ * 3);
    nlIntensity = (float *)malloc(sizeof(float) * PLAINSZ);
    checker = (float *)malloc(sizeof(float) * PLAINSZ);

    cl_int side = WIDTH;

    clock_t start,end;
    start = clock();

    // プラットフォーム一覧を取得
    cl_platform_id platforms[PLATFORM_MAX];
    cl_uint platformCount;

    EC(clGetPlatformIDs(PLATFORM_MAX, platforms, &platformCount), "clGetPlatformIDs");
    if (platformCount == 0) {
        std::cerr << "No platform.\n";
//        return EXIT_FAILURE;
    }

    // 見つかったプラットフォームの情報を印字
//    for (int i = 0; i < platformCount; i++) {
//        char vendor[100] = {0};
//        char version[100] = {0};
//        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, nullptr), "clGetPlatformInfo");
//        EC(clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, nullptr), "clGetPlatformInfo");
////        std::cout << "Platform id: " << platforms[i] << ", Vendor: " << vendor << ", Version: " << version << "\n";
//    }

    // デバイス一覧を取得
    cl_device_id devices[DEVICE_MAX];
    cl_uint deviceCount;
    EC(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, DEVICE_MAX, devices, &deviceCount), "clGetDeviceIDs");
    if (deviceCount == 0) {
        std::cerr << "No device.\n";
//        return EXIT_FAILURE;
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
    char fileName[] = "./calcLightSource.cl";
    fp = fopen(fileName, "r");
    if(!fp) {
        std::cout << "no file.\n";
        exit(1);
    }

    char *source_str;
    size_t source_size;
    source_str = (char*)malloc(0x5000);
//    char* source_str = new char[0x10000];
    source_size = fread(source_str, 1, 0x5000, fp);
    fclose(fp);
//    std::cout << source_str <<std::endl;

    // プログラムのビルド
    cl_program program = NULL;
    program = clCreateProgramWithSource(ctx, 1, (const char **)&source_str, (const size_t *)&source_size, &err);
    clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    // カーネルの作成
    cl_kernel kernel = clCreateKernel(program, "lightSource", &err);
    EC2("clCreateKernel");

    // デバイスメモリを確保しつつデータをコピ-
    // make buffer
    cl_mem mempGrad;
    cl_mem mempfColor;
    cl_mem mempfMedium;
    cl_mem mempfOpacity;
    cl_mem memplColor;
    cl_mem memplDirection;
    cl_mem memplIntensity;
    cl_mem memnlColor;
    cl_mem memnlDirection;
    cl_mem memnlIntensity;
    cl_mem memChecker;
    EC2("clCreateBuffer");

//    for (int i=0; i<FIELDSZ; i++){
//        std::cout << grad[i * 3] << " ";
//    }
//    std::cout << std::endl << std::endl;

    // コマンドキューの作成
    cl_command_queue q = clCreateCommandQueue(ctx, devices[0], 0, &err);

    for (unsigned int plain=0; plain<DEPTH; ++plain) {
        // write buffer
//        std::cout << "plain: " << plain << std::endl;
        if (plain == 0) {
            for (unsigned int j=0; j<HEIGHT; ++j) {
                for (unsigned int k=0; k < WIDTH; ++k) {
                    unsigned coordinate = (j + k * WIDTH);
                    setVec3Float(&lColor[coordinate * 3], color);
                    lIntensity[coordinate] = 1.0;
                    setVec3Float(&lDirection[coordinate * 3], direction);
                }
            }
        } else {

            for (int i=0; i<PLAINSZ; i++){
                setVec3Float(&pfColor[i * 3], &fColor[(plain - 1) * PLAINSZ * 3 + i * 3]);
                setVec3Float(&pGrad[i * 3], &grad[(plain - 1) * PLAINSZ * 3 + i * 3]);
                setVec3Float(&pfMedium[i * 3], &fMedium[(plain - 1) * PLAINSZ * 3 + i * 3]);
                setVec3Float(&plColor[i * 3], &lColor[(plain - 1) * PLAINSZ * 3 + i * 3]);
                setVec3Float(&plDirection[i * 3], &lDirection[(plain - 1) * PLAINSZ * 3 + i * 3]);
                plIntensity[i] = lIntensity[(plain - 1) * PLAINSZ + i];
                pfOpacity[i] = fOpacity[(plain - 1) * PLAINSZ + i];
            }

            mempGrad = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, pGrad, &err);
            mempfColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, pfColor, &err);
            mempfMedium = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, pfMedium, &err);
            mempfOpacity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ, pfOpacity, &err);
            memplColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, plColor, &err);
            memplDirection = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, plDirection, &err);
            memplIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ, plIntensity, &err);
            memnlColor = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, nlColor, &err);
            memnlDirection = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ*3, nlDirection, &err);
            memnlIntensity = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float)*PLAINSZ, nlIntensity, &err);
            memChecker = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * PLAINSZ, checker, &err);
            EC2("clCreateBuffer");

            EC(clSetKernelArg(kernel, 0, sizeof(cl_mem), &mempGrad), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 1, sizeof(cl_mem), &mempfColor), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 2, sizeof(cl_mem), &mempfMedium), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 3, sizeof(cl_mem), &mempfOpacity), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 4, sizeof(cl_mem), &memplColor), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 5, sizeof(cl_mem), &memplDirection), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 6, sizeof(cl_mem), &memplIntensity), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 7, sizeof(cl_mem), &memnlColor), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 8, sizeof(cl_mem), &memnlDirection), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 9, sizeof(cl_mem), &memnlIntensity), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 10, sizeof(cl_int), &side), "clSetKernelArg");
            EC(clSetKernelArg(kernel, 11, sizeof(cl_mem), &memChecker), "clSetKernelArg");

            EC2("clCreateCommandQueue");

//            size_t local_item_size = WIDTH;
            size_t global_item_size = WIDTH * WIDTH;

            EC(clEnqueueNDRangeKernel(q, kernel, 1, nullptr, &global_item_size, nullptr, 0, nullptr, nullptr), "clEnqueueNDRangeKernel");

            // 結果を読み込み
            EC(clEnqueueReadBuffer(q, memnlColor, CL_TRUE, 0, sizeof(float) * PLAINSZ * 3, nlColor, 0, nullptr, nullptr), "clEnqueueReadBuffer");
            EC(clEnqueueReadBuffer(q, memnlDirection, CL_TRUE, 0, sizeof(float) * PLAINSZ * 3, nlDirection, 0, nullptr, nullptr), "clEnqueueReadBuffer");
            EC(clEnqueueReadBuffer(q, memnlIntensity, CL_TRUE, 0, sizeof(float) * PLAINSZ, nlIntensity, 0, nullptr, nullptr), "clEnqueueReadBuffer");
            EC(clEnqueueReadBuffer(q, memChecker, CL_TRUE, 0, sizeof(float) * PLAINSZ, checker, 0, nullptr, nullptr), "clEnqueueReadBuffer");

//            std::cout << "new      " << nlDirection[(43 + 98 * WIDTH) * 3] << " " << checker[(43 + 98 * WIDTH)] << std::endl;

//            lightSmoothing(nlColor);
//            lightSmoothing(nlDirection);

            for (unsigned int j=0; j<HEIGHT; ++j) {
                for (unsigned int k = 0; k < WIDTH; ++k) {
                    setVec3Float(&lColor[(plain * PLAINSZ + j + k * WIDTH) * 3], &nlColor[(j + k * WIDTH) * 3]);
                    setVec3Float(&lDirection[(plain * PLAINSZ + j + k * WIDTH) * 3], &nlDirection[(j + k * WIDTH) * 3]);
                    lIntensity[plain * PLAINSZ + j + k * WIDTH] = nlIntensity[j + k * WIDTH];
                }
            }
        }
    }

    end = clock();
    printf("%lf seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

    // コマンドキューの解放
    EC(clReleaseCommandQueue(q), "clReleaseCommandQueue");
    // デバイスメモリを解放
    EC(clReleaseMemObject(mempGrad), "clReleaseMemObject");
    EC(clReleaseMemObject(mempfColor), "clReleaseMemObject");
    EC(clReleaseMemObject(mempfMedium), "clReleaseMemObject");
    EC(clReleaseMemObject(mempfOpacity), "clReleaseMemObject");
    EC(clReleaseMemObject(memplColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memplDirection), "clReleaseMemObject");
    EC(clReleaseMemObject(memplIntensity), "clReleaseMemObject");
    EC(clReleaseMemObject(memnlColor), "clReleaseMemObject");
    EC(clReleaseMemObject(memnlDirection), "clReleaseMemObject");
    EC(clReleaseMemObject(memnlIntensity), "clReleaseMemObject");

    // カーネルの解放
    EC(clReleaseKernel(kernel), "clReleaseKernel");
    // プログラムの解放
    EC(clReleaseProgram(program), "clReleaseProgram");
    // コンテキストの解放
    EC(clReleaseContext(ctx), "clReleaseContext");

    free(source_str);
    free(pGrad);
    free(pfColor);
    free(pfMedium);
    free(pfOpacity);
    free(plColor);
    free(plDirection);
    free(plIntensity);
    free(nlColor);
    free(nlDirection);
    free(nlIntensity);
}


//void bilinearInterpolation(float *origin, float *p1, float *p2, float *p3, float *p4, lpProperty &lp){
////    std::cout << p1 << p2 << p3 << p4 << std::endl;
//
//    float x1[3], x2[3], y1[3], y2[3];
//    float d1[3], d2[3], c1[3], c2[3], m1[3], m2[3];
//    float o1, o2, i1, i2;
//    setVec3Float(x1, p1);
//    float dis1 = vectorDistance(p1, p2);
//    float dis2 = vectorDistance(p1, p3);
//    float dis3 = vectorDistance(p1, p4);
//    if ((dis1 > dis2) || (dis1 > dis3) ) {
//        setVec3Float(x2, p1);
//        setVec3Float(y1, p4);
//        setVec3Float(y2, p2);
//    } else if ((dis2 > dis1) || (dis2 > dis3)){
//        setVec3Float(y2, p3);
//        setVec3Float(x2, p2);
//        setVec3Float(y1, p4);
//    } else {
//        setVec3Float(y2, p4);
//        setVec3Float(x2, p2);
//        setVec3Float(y1, p3);
//    }
//
//    // 座標がマイナスなら保管
//    checkMinus(x1);
//    checkMinus(x2);
//    checkMinus(y1);
//    checkMinus(y2);
//
//    // get direction vector like (0, 0, 1)
//    float dic1[3], dic2[3], ratio1[3], ratio2[3], multi1[3], multi2[3], abs[3];
//    subVec(x2, x1, dic1);
//    subVec(y1, x1, dic2);
//    multiVec(origin, dic1, multi1);
//    multiVec(x1, dic1, multi2);
//    subVec(multi1, multi2, ratio1);
//    absVec3(ratio1);
//    multiVec(origin, dic2, multi1);
//    multiVec(x1, dic2, multi2);
//    subVec(multi1, multi2, ratio2);
//    absVec3(ratio2);
//
//    float unit[] = {1, 1, 1};
//    float vec1[3], vec2[3];
//    multiFloat(&lDirection[getCoordinate(x1)], (1 - inpro(unit, ratio1)), vec1);
//    multiFloat(&lDirection[getCoordinate(x2)], inpro(unit, ratio1), vec2);
//    addVec(vec1, vec2, d1);
//    multiFloat(&lDirection[getCoordinate(y1)], (1 - inpro(unit, ratio1)), vec1);
//    multiFloat(&lDirection[getCoordinate(y2)], inpro(unit, ratio1), vec2);
//    addVec(vec1, vec2, d2);
//
//    multiFloat(&lColor[getCoordinate(x1)], (1 - inpro(unit, ratio1)), vec1);
//    multiFloat(&lColor[getCoordinate(x2)], inpro(unit, ratio1), vec2);
//    addVec(vec1, vec2, c1);
//    multiFloat(&lColor[getCoordinate(y1)], (1 - inpro(unit, ratio1)), vec1);
//    multiFloat(&lColor[getCoordinate(y2)], inpro(unit, ratio1), vec2);
//    addVec(vec1, vec2, c2);
//
//    i1 = lIntensity[getCoordinate(x1)] * (1 - inpro(unit, ratio1)) + lIntensity[getCoordinate(x2)] * inpro(unit, ratio1);
//    i2 = lIntensity[getCoordinate(y1)] * (1 - inpro(unit, ratio1)) + lIntensity[getCoordinate(y2)] * inpro(unit, ratio1);
//
//    o1 = fOpacity[getCoordinate(x1)] * (1 - inpro(unit, ratio1)) + fOpacity[getCoordinate(x2)] * inpro(unit, ratio1);
//    o2 = fOpacity[getCoordinate(y1)] * (1 - inpro(unit, ratio1)) + fOpacity[getCoordinate(y2)] * inpro(unit, ratio1);
//
//    multiFloat(&fMedium[getCoordinate(x1)], (1 - inpro(unit, ratio1)), vec1);
//    multiFloat(&fMedium[getCoordinate(x2)], inpro(unit, ratio1), vec2);
//    addVec(vec1, vec2, m1);
//    multiFloat(&fMedium[getCoordinate(y1)], (1 - inpro(unit, ratio1)), vec1);
//    multiFloat(&fMedium[getCoordinate(y2)], inpro(unit, ratio1), vec2);
//    addVec(vec1, vec2, m2);
//
//    multiFloat(d1, 1 - inpro(unit, ratio2), vec1);
//    multiFloat(d2, inpro(unit, ratio2), vec2);
//    addVec(vec1, vec2, lp.lightDirection);
//
//    multiFloat(c1, 1 - inpro(unit, ratio2), vec1);
//    multiFloat(c2, inpro(unit, ratio2), vec2);
//    addVec(vec1, vec2, lp.lightDirection);
//
//    lp.opacity =  o1 * (1 - inpro(unit, ratio2)) + o2 * inpro(unit, ratio2);
//    lp.intensity =  i1 * (1 - inpro(unit, ratio2)) + i2 * inpro(unit, ratio2);
//
//    multiFloat(m1, 1 - inpro(unit, ratio2), vec1);
//    multiFloat(m2, inpro(unit, ratio2), vec2);
//    addVec(vec1, vec2, lp.medium);
//
//    //    std::cout << o1 << ", " << o2<< ", " << inpro(Vec3f(1, 1, 1), ratio1) << lp.lightColor << lp.opacity << lp.medium << lp.intensity << std::endl;
//}
//
//void nextDirection(lpProperty &lp, unsigned int coordinate, unsigned int previousCoo, float *direction,
//                   unsigned int maxDir) {
////    ?????????????????
//    float previousDirection[3];
//    addVec(&lDirection[previousCoo], &grad[previousCoo], previousDirection);
////    subVec(&lDirection[previousCoo], &grad[previousCoo], previousDirection);
//
//    float backPoint[3], p1[3], p2[3], p3[3], p4[3], origin[3], coeff;
//    float backVector[3], coordVec[3];
//    bool flagInt;
//
//    if (maxDir == 0) {
//        coeff = 1 / previousDirection[0];
//        multiFloat(previousDirection, coeff, backVector);
//        getVector(coordinate, coordVec);
//        subVec(coordVec, backVector, backPoint);
//        setVec3Float(origin, backPoint);
//        origin[0] = (int) origin[0];
//        checkMinus(backPoint);
//        flagInt = checkInteger(backPoint);
//        backPoint[0] = (int) backPoint[0];
//        backPoint[1] = (int) backPoint[1];
//        backPoint[2] = (int) backPoint[2];
//        setVec3Float(p1, backPoint);
//        setVec3Float(p2, backPoint);
//        setVec3Float(p3, backPoint);
//        setVec3Float(p4, backPoint);
//        p2[1] += 1;
//        p3[1] += 1;
//        p3[2] += 1;
//        p4[2] += 1;
//    } else if (maxDir == 1) {
//        coeff = 1 / previousDirection[1];
//        multiFloat(previousDirection, coeff, backVector);
//        getVector(coordinate, coordVec);
//        subVec(coordVec, backVector, backPoint);
//        setVec3Float(origin, backPoint);
//        origin[1] = (int) origin[1];
//        checkMinus(backPoint);
//        flagInt = checkInteger(backPoint);
//        backPoint[0] = (int) backPoint[0];
//        backPoint[1] = (int) backPoint[1];
//        backPoint[2] = (int) backPoint[2];
//        setVec3Float(p1, backPoint);
//        setVec3Float(p2, backPoint);
//        setVec3Float(p3, backPoint);
//        setVec3Float(p4, backPoint);
//        p2[0] += 1;
//        p3[0] += 1;
//        p3[2] += 1;
//        p4[2] += 1;
//    } else if (maxDir == 2) {
//        coeff = 1 / previousDirection[2];
//        multiFloat(previousDirection, coeff, backVector);
//        getVector(coordinate, coordVec);
//        subVec(coordVec, backVector, backPoint);
//        setVec3Float(origin, backPoint);
//        origin[2] = (int) origin[2];
//        checkMinus(backPoint);
//        flagInt = checkInteger(backPoint);
//        backPoint[0] = (int) backPoint[0];
//        backPoint[1] = (int) backPoint[1];
//        backPoint[2] = (int) backPoint[2];
//        setVec3Float(p1, backPoint);
//        setVec3Float(p2, backPoint);
//        setVec3Float(p3, backPoint);
//        setVec3Float(p4, backPoint);
//        p2[0] += 1;
//        p3[0] += 1;
//        p3[1] += 1;
//        p4[1] += 1;
//    }
//    if (flagInt) {
////        std::cout << "int" << std::endl;
//        checkMinus(backPoint);
//        setVec3Float(lp.lightDirection, &lDirection[getCoordinate(backPoint)]);
//        setVec3Float(lp.lightColor, &lColor[getCoordinate(backPoint)]);
//        lp.opacity = fOpacity[getCoordinate(backPoint)];
//        lp.intensity = lIntensity[getCoordinate(backPoint)];
//        setVec3Float(lp.medium, &fMedium[getCoordinate(backPoint)]);
////        std::cout << lp.lightColor << getVector(coordinate) << backPoint << lp.opacity << lp.medium << lp.intensity << std::endl;
//    } else {
////        std::cout << "interpolation" << getVector(coordinate) << grad[previousCoo] << previousDirection << coeff << ", " << maxDir << std::endl;
//        bilinearInterpolation(origin, p1, p2, p3, p4, lp);
//    }
//}
//
//void calcLightSource(float *direction, float *color){
//    unsigned int coordinate, previousCoo, maxDir;
//    float vec1[3];
//    if ((direction[0] >= direction[1]) && (direction[0] >= direction[2])){
//        maxDir = 0;
//    } else if ((direction[1] >= direction[0]) && (direction[1] >= direction[2])){
//        maxDir = 1;
//    } else {
//        maxDir = 2;
//    }
//    for (unsigned int i=0; i<WIDTH; ++i){
//        for (unsigned int j=0; j<HEIGHT; ++j){
//            for (unsigned int k=0; k<DEPTH; ++k){
//                coordinate = (i * (int)pow(WIDTH, maxDir) + j * (int)pow(WIDTH, (maxDir==0)?2:(maxDir - 1)) + k * (int)pow(WIDTH, (maxDir<2)?(maxDir + 1):(0))) * 3;
//                if (i == 0){
//                    setVec3Float(&lColor[coordinate], color);
//                    lIntensity[coordinate] = 1.;
//                    setVec3Float(&lDirection[coordinate], direction);
//                } else {
//                    lpProperty lp;
//                    previousCoo = coordinate - (int)pow(WIDTH, maxDir) * 3;
//                    nextDirection(lp, coordinate, previousCoo, direction, maxDir);
////                    std::cout << i << j << k << lp.lightColor << lp.lightDirection << lp.medium << lp.opacity << lp.intensity << std::endl;
//                    multiVec(lp.lightColor, lp.medium, vec1);
//                    multiFloat(vec1, (1 - lp.opacity) * lp.intensity, &lColor[coordinate]);
//                    setVec3Float(&lDirection[coordinate], lp.lightDirection);
//                    lIntensity[coordinate] = lp.intensity;
//                }
//            }
//        }
//    }
//}