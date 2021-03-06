//
// Created by Nozomi on 2019-05-11.
//

#include "objectData.h"
#include "variables.h"
#include "myMath.h"
#include "vector.h"
#include "stdio.h"
#include "cmath"
#include <iostream>

// position, radius, surface color, reflectivity, transparency, refractivity, emission color
void inSphere(float *center, float radius, float *surfaceColor, float reflectivity, float opacity, float refractivity, float *emissionColor){
    for (unsigned int i=0; i < WIDTH; ++i){
        for (unsigned int j=0; j<HEIGHT; ++j){
            for (unsigned int k=0; k<DEPTH; ++k){
                if (((i - center[0]) * (i - center[0]) + (j - center[1]) * (j - center[1]) + (k - center[2]) * (k - center[2])) > radius * radius){
                    continue;
                }
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = surfaceColor[0];
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = surfaceColor[1];
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = surfaceColor[2];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = surfaceColor[0];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = surfaceColor[1];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = surfaceColor[2];
//                fReflectivity[i + j * WIDTH + k * WIDTH * HEIGHT].reflectivity = reflectivity;
                fOpacity[(i + j * WIDTH + k * WIDTH * HEIGHT)] = opacity;
//                f[i + j * WIDTH + k * WIDTH * HEIGHT) * 3].emissionColor = emissionColor;
                fRefractivity[(i + j * WIDTH + k * WIDTH * HEIGHT)] = refractivity;
            }
        }
    }
}

void inconstantSphere(float *center, float radius, float *surfaceColor, float reflectivity, float opacity, float maxRefractivity, float *emissionColor){
    float distance;
    for (unsigned int i=0; i < WIDTH; ++i){
        for (unsigned int j=0; j<HEIGHT; ++j){
            for (unsigned int k=0; k<DEPTH; ++k){
                distance = (i - center[0]) * (i - center[0]) + (j - center[1]) * (j - center[1]) + (k - center[2]) * (k - center[2]);
                if (distance > radius * radius){
                    continue;
                }
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = surfaceColor[0];
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = surfaceColor[1];
                fColor[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = surfaceColor[2];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3] = surfaceColor[0];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 1] = surfaceColor[1];
                fMedium[(i + j * WIDTH + k * WIDTH * HEIGHT) * 3 + 2] = surfaceColor[2];
//                fReflectivity[i + j * WIDTH + k * WIDTH * HEIGHT].reflectivity = reflectivity;
                fOpacity[(i + j * WIDTH + k * WIDTH * HEIGHT)] = opacity;
                fRefractivity[(i + j * WIDTH + k * WIDTH * HEIGHT)] = (maxRefractivity - 1.) * (1 - distance / (radius * radius)) + 1.;
            }
        }
    }
}

//void setSurface(float *p1, float *p2, float *p3){
//    // p1, p2, p3を外側に向かって反時計回り（右ねじ）
//    float vec1[3], vec2[3], cross[3];
//    surfaces.push_back(p1[0]);
//    surfaces.push_back(p1[1]);
//    surfaces.push_back(p1[2]);
//    surfaces.push_back(p2[0]);
//    surfaces.push_back(p2[1]);
//    surfaces.push_back(p2[2]);
//    surfaces.push_back(p3[0]);
//    surfaces.push_back(p3[1]);
//    surfaces.push_back(p3[2]);
//
//    subVec(p2, p1, vec1);
//    subVec(p3, p1, vec2);
//    cross[0] =  vec1[1] * vec2[2] - vec1[2] * vec2[1];
//    cross[1] =  vec1[2] * vec2[0] - vec1[0] * vec2[2];
//    cross[2] =  vec1[0] * vec2[1] - vec1[1] * vec2[0];
//    vecNormalize(cross, cross);
//    surfaces.push_back(cross[0]);
//    surfaces.push_back(cross[1]);
//    surfaces.push_back(cross[2]);
//}

void inPlain (float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float refractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity){
    unsigned int dis1 = std::floor(vectorDistance(p1, p2)) + 1;
    unsigned int dis2 = std::floor(vectorDistance(p1, p3)) + 1;
    float vec1[3], vec2[3], sub1[3], sub2[3];
    int count = 0;
//    printf("inplain\n");
//    std::cout << p1[0] << " " << p1[1] << " " << p1[2] << " "<< p2[0] << " " << p2[1] << " " << p2[2] << " " << p3[0] << " " << p3[1] << " " << p3[2] << " " << std::endl;
    float calcStep = 1;
    subVec(p2, p1, sub1);
    subVec(p3, p1, sub2);
    vecNormalize(sub1, vec1);
    vecNormalize(sub2, vec2);
    divFloat(vec1, calcStep, vec1);
    divFloat(vec2, calcStep, vec2);
    float current[3];
    float integer[3];
    for (unsigned int x=0; x < dis1 * calcStep; ++x){
        for (unsigned int y=0; y < dis2 * calcStep; ++y){
            current[0] = p1[0] + vec1[0] * x + vec2[0] * y;
            current[1] = p1[1] + vec1[1] * x + vec2[1] * y;
            current[2] = p1[2] + vec1[2] * x + vec2[2] * y;
            integer[0] = roundInt(current[0]);
            integer[1] = roundInt(current[1]);
            integer[2] = roundInt(current[2]);
            if ((integer[0] < 0) || (integer[0] > WIDTH - 1) || (integer[1] < 0) || (integer[1] > HEIGHT - 1) || (integer[2] < 0) || (integer[2] > DEPTH - 1)){
                continue;
            } else {
//                std::cout << " " << dis2 << " " << integer[0] << " " << integer[1] << " " << integer[2] << std::endl;
                setVec3Float(&fColor[getCoordinate(integer)], color);
//                std::cout << integer[0] << " " << integer[1] << " " << integer[2] << " " << getCoordinate(integer) << std::endl;
//                setVec3Float(&fMedium[getCoordinate(integer)], color);
                fMedium[getCoordinate(integer)] = color[0];
                fMedium[getCoordinate(integer) + 1] = color[1];
                fMedium[getCoordinate(integer) + 2] = color[2];
//                fRelectivity[getCoordinate(integer)] = reflectivity;
                fOpacity[getScalarCoo(integer)] = opacity;
                fRefractivity[getScalarCoo(integer)] = refractivity;
                count += 1;
            }
        }
    }
//    setSurface(p1, p2, p3);
//    setSurface(p1, p4, p3);
}

void inconstantRectangle(float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float maxRefractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity){
    unsigned int dis1 = std::floor(vectorDistance(p1, p2));
    unsigned int dis2 = std::floor(vectorDistance(p1, p3));
    unsigned int dis3 = std::floor(vectorDistance(p1, p4));
    float vec1[3], vec2[3], vec3[3], sub1[3], sub2[3], sub3[3], calcStep = 5,current[3],integer[3], center[3], distanceSq, radius;
    subVec(p2, p1, sub1);
    subVec(p3, p1, sub2);
    subVec(p4, p1, sub3);
    vecNormalize(sub1, vec1);
    vecNormalize(sub2, vec2);
    vecNormalize(sub3, vec3);
    center[0] = p1[0] + sub1[0] / 2 + sub2[0] / 2 + sub3[0] / 2;
    center[1] = p1[1] + sub1[1] / 2 + sub2[1] / 2 + sub3[1] / 2;
    center[2] = p1[2] + sub1[2] / 2 + sub2[2] / 2 + sub3[2] / 2;
    radius = vectorDistance(center, p1);
    for (int x=0; x < dis1 * calcStep + 1; ++x) {
        for (int y = 0; y < dis2 * calcStep + 1; ++y) {
            for (int z = 0; z < dis3 * calcStep + 1; ++z){
                current[0] = p1[0] + vec1[0] / calcStep * x + vec2[0] / calcStep * y + vec3[0] / calcStep * z;
                current[1] = p1[1] + vec1[1] / calcStep * x + vec2[1] / calcStep * y + vec3[1] / calcStep * z;
                current[2] = p1[2] + vec1[2] / calcStep * x + vec2[2] / calcStep * y + vec3[2] / calcStep * z;
                integer[0] = roundInt(current[0]);
                integer[1] = roundInt(current[1]);
                integer[2] = roundInt(current[2]);
                distanceSq = (integer[0] - center[0]) * (integer[0] - center[0]) + (integer[1] - center[1]) * (integer[1] - center[1]) + (integer[2] - center[2]) * (integer[2] - center[2]);
                if ((integer[0] < 0) || (integer[0] > WIDTH - 1) || (integer[1] < 0) || (integer[1] > HEIGHT - 1) ||
                    (integer[2] < 0) || (integer[2] > DEPTH - 1)) {
                    continue;
                } else {
//                    std::cout << "inrec " << getCoordinate(integer) <<" " << fColor[getCoordinate(integer)] << " " << opacity << std::endl;
                    setVec3Float(&fColor[getCoordinate(integer)], color);
                    setVec3Float(&fMedium[getCoordinate(integer)], color);
//                    fReflectivity[getCoordinate(integer)] = reflectivity;
                    fOpacity[getScalarCoo(integer)] = opacity;
                    fRefractivity[getScalarCoo(integer)] = 1 + (1 - sqrt(distanceSq) / radius) * (maxRefractivity - 1.);
                }
            }
        }
    }
    // set surfaces
//    float q1[3], q2[3], q3[3], q4[3];
//    subVec(p2, p1, sub1);
//    subVec(p3, p1, sub2);
//    subVec(p4, p1, sub3);
//    addVec(sub1, sub2, vec1);
//    addVec(sub1, sub3, vec2);
//    addVec(p3, vec2, q1);
//    addVec(p4, sub2, q2);
//    addVec(p1, vec2, q3);
//    addVec(p2, sub2, q4);

//    setSurface(p1, p4, p3);
//    setSurface(q2, p3, p4);
//    setSurface(p1, p3, p2);
//    setSurface(q4, p2, p3);
//    setSurface(p1, p2, p4);
//    setSurface(q3, p4, p2);
//
//    setSurface(q1, q4, q2);
//    setSurface(p3, q2, q4);
//    setSurface(q1, q2, q3);
//    setSurface(p4, q3, q2);
//    setSurface(q1, q3, q4);
//    setSurface(p2, q4, q3);
}

void inRectangle(float *p1, float *p2, float *p3, float *p4, float *color, float reflectivity, float opacity, float refractivity, float *fColor, float *fMedium, float *fOpacity, float *fRefractivity){
    unsigned int dis1 = std::floor(vectorDistance(p1, p2));
    unsigned int dis2 = std::floor(vectorDistance(p1, p3));
    unsigned int dis3 = std::floor(vectorDistance(p1, p4));
    float vec1[3], vec2[3], vec3[3], sub1[3], sub2[3], sub3[3], calcStep = 5;
    subVec(p2, p1, sub1);
    subVec(p3, p1, sub2);
    subVec(p4, p1, sub3);
//    std::cout << sub3[0] << sub3[1] << sub3[2] << std::endl;
    vecNormalize(sub1, vec1);
    vecNormalize(sub2, vec2);
    vecNormalize(sub3, vec3);
    float current[3];
    float integer[3];
    for (int x=0; x < dis1 * calcStep + 1; ++x) {
        for (int y = 0; y < dis2 * calcStep + 1; ++y) {
            for (int z = 0; z < dis3 * calcStep + 1; ++z){
                current[0] = p1[0] + vec1[0] / calcStep * x + vec2[0] / calcStep * y + vec3[0] / calcStep * z;
                current[1] = p1[1] + vec1[1] / calcStep * x + vec2[1] / calcStep * y + vec3[1] / calcStep * z;
                current[2] = p1[2] + vec1[2] / calcStep * x + vec2[2] / calcStep * y + vec3[2] / calcStep * z;
                integer[0] = roundInt(current[0]);
                integer[1] = roundInt(current[1]);
                integer[2] = roundInt(current[2]);
                if ((integer[0] < 0) || (integer[0] > WIDTH - 1) || (integer[1] < 0) || (integer[1] > HEIGHT - 1) ||
                    (integer[2] < 0) || (integer[2] > DEPTH - 1)) {
                    continue;
                } else {
//                    std::cout << "inrec " << getCoordinate(integer) <<" " << fColor[getCoordinate(integer)] << " " << opacity << std::endl;
                    setVec3Float(&fColor[getCoordinate(integer)], color);
                    setVec3Float(&fMedium[getCoordinate(integer)], color);
//                    fReflectivity[getCoordinate(integer)] = reflectivity;
                    fOpacity[getScalarCoo(integer)] = opacity;
                    fRefractivity[getScalarCoo(integer)] = refractivity;
                }
            }
        }
    }
    // set surfaces
    float q1[3], q2[3], q3[3], q4[3];
    subVec(p2, p1, sub1);
    subVec(p3, p1, sub2);
    subVec(p4, p1, sub3);
    addVec(sub1, sub2, vec1);
    addVec(sub1, sub3, vec2);
    addVec(p3, vec2, q1);
    addVec(p4, sub2, q2);
    addVec(p1, vec2, q3);
    addVec(p2, sub2, q4);

//    setSurface(p1, p4, p3);
//    setSurface(q2, p3, p4);
//    setSurface(p1, p3, p2);
//    setSurface(q4, p2, p3);
//    setSurface(p1, p2, p4);
//    setSurface(q3, p4, p2);
//
//    setSurface(q1, q4, q2);
//    setSurface(p3, q2, q4);
//    setSurface(q1, q2, q3);
//    setSurface(p4, q3, q2);
//    setSurface(q1, q3, q4);
//    setSurface(p2, q4, q3);
}